#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <CronAlarms.h>

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Web server
AsyncWebServer server(80);

// EEPROM addresses for job data storage
const int ADDR_JOB_COUNT = 0;
const int ADDR_JOB_BASE = 10;

// Maximum number of jobs
const int MAX_JOBS = 10;

// Structure to hold job data
struct JobData {
  bool enabled;
  char cronSchedule[50];
  char url[255];
};

JobData jobs[MAX_JOBS];  // Array to hold all jobs
CronAlarms cron;         // CronAlarms instance for job scheduling

// Function prototypes
void loadJobsFromEEPROM();
void saveJobsToEEPROM();
void handleRoot();
void handleSaveJob();
void handleJobList();
void handleNotFound();

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  while (!Serial) {}

  // Initialize EEPROM
  EEPROM.begin(512);

  // Load jobs from EEPROM
  loadJobsFromEEPROM();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize time library
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    delay(1000);
    Serial.println("Waiting for time synchronization...");
  }
  Serial.println("Time synchronized");

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/savejob", HTTP_POST, handleSaveJob);
  server.on("/joblist", HTTP_GET, handleJobList);
  server.onNotFound(handleNotFound);

  // Start web server
  server.begin();

  // Print server IP address
  Serial.print("Server running at ");
  Serial.println(WiFi.localIP());

  // Schedule the jobs
  for (int i = 0; i < MAX_JOBS; i++) {
    if (jobs[i].enabled) {
      cron.setAlarm(i, jobs[i].cronSchedule, executeJob);
    }
  }
}

void loop() {
  // Handle web server requests
  server.handleClient();
  // Update the cron scheduler
  cron.run();
}

void executeJob() {
  // Get the job index from the CronAlarms instance
  int jobIndex = cron.getTriggeredAlarmId();

  // Get the corresponding job data
  JobData job = jobs[jobIndex];

  // Perform the job action, in this case, making an HTTP request
  // using the specified URL
  Serial.println("Executing job: " + String(job.url));

  // TODO: Add your code here to execute the job

  // Print the next runtime of the job
  time_t nextRun = cron.getTriggerTime(jobIndex);
  Serial.print("Next run: ");
  Serial.println(String(day(nextRun)) + "-" + String(month(nextRun)) + "-" + String(year(nextRun)) + " " +
                 String(hour(nextRun)) + ":" + String(minute(nextRun)) + ":" + String(second(nextRun)));
}

void loadJobsFromEEPROM() {
  int jobCount = EEPROM.read(ADDR_JOB_COUNT);
  if (jobCount > MAX_JOBS) {
    jobCount = MAX_JOBS;
  }

  for (int i = 0; i < jobCount; i++) {
    int addr = ADDR_JOB_BASE + (i * sizeof(JobData));
    EEPROM.get(addr, jobs[i]);
  }
}

void saveJobsToEEPROM() {
  EEPROM.write(ADDR_JOB_COUNT, MAX_JOBS);

  for (int i = 0; i < MAX_JOBS; i++) {
    int addr = ADDR_JOB_BASE + (i * sizeof(JobData));
    EEPROM.put(addr, jobs[i]);
  }

  EEPROM.commit();
}

void handleRoot() {
  String html =
      "<html><body><h1>Cron Server</h1>"
      "<form action=\"/savejob\" method=\"post\">"
      "<input type=\"text\" name=\"schedule\" placeholder=\"Cron schedule\"><br>"
      "<input type=\"text\" name=\"url\" placeholder=\"URL\"><br>"
      "<input type=\"submit\" value=\"Save Job\">"
      "</form>"
      "<a href=\"/joblist\">View Job List</a>"
      "</body></html>";

  server.send(200, "text/html", html);
}

void handleSaveJob() {
  String schedule = server.arg("schedule");
  String url = server.arg("url");

  // Validate cron schedule
  if (!cron.validate(schedule.c_str())) {
    server.send(400, "text/plain", "Invalid cron schedule");
    return;
  }

  // Find an empty slot for the new job
  int emptySlot = -1;
  for (int i = 0; i < MAX_JOBS; i++) {
    if (!jobs[i].enabled) {
      emptySlot = i;
      break;
    }
  }

  // No empty slot available
  if (emptySlot == -1) {
    server.send(400, "text/plain", "Maximum number of jobs reached");
    return;
  }

  // Store the new job data
  jobs[emptySlot].enabled = true;
  strncpy(jobs[emptySlot].cronSchedule, schedule.c_str(), sizeof(jobs[emptySlot].cronSchedule));
  strncpy(jobs[emptySlot].url, url.c_str(), sizeof(jobs[emptySlot].url));

  // Save the jobs to EEPROM
  saveJobsToEEPROM();

  // Schedule the new job
  cron.setAlarm(emptySlot, jobs[emptySlot].cronSchedule, executeJob);

  server.send(200, "text/plain", "Job saved successfully");
}

void handleJobList() {
  String jobList = "<html><body><h1>Job List</h1>";

  for (int i = 0; i < MAX_JOBS; i++) {
    if (jobs[i].enabled) {
      time_t nextRun = cron.getTriggerTime(i);
      String nextRunStr = String(day(nextRun)) + "-" + String(month(nextRun)) + "-" + String(year(nextRun)) + " " +
                          String(hour(nextRun)) + ":" + String(minute(nextRun)) + ":" + String(second(nextRun));

      jobList += "<p>Job " + String(i + 1) + ": " + jobs[i].url + " (Next run: " + nextRunStr + ")</p>";
    }
  }

  jobList += "</body></html>";

  server.send(200, "text/html", jobList);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}
