# ESP8266 Cron Server with Web Interface

This project implements an ESP8266-based Cron server with a web interface. It allows users to schedule jobs in cron format along with corresponding URLs. The ESP8266 device will execute the specified URL at the scheduled time. The project includes features such as schedule validation, displaying the next runtime, and storing job data to memory for recovery after power-off.

## Getting Started

### Prerequisites

To run this project, you will need the following:

- Arduino IDE
- ESP8266 board support for Arduino IDE
- ESPAsyncTCP library
- ESPAsyncWebServer library
- CronAlarms library

### Installation

1. Clone or download the project repository to your local machine.
2. Open the Arduino IDE and install the ESP8266 board support if you haven't already.
3. Install the required libraries by navigating to **Sketch -> Include Library -> Manage Libraries** and search for each library name mentioned in the prerequisites section. Click on the library and select "Install".
4. Connect your ESP8266 device to your computer using a USB cable.
5. Open the `esp8266_cron_server.ino` file in the Arduino IDE.
6. Modify the `ssid` and `password` variables in the code to match your Wi-Fi credentials.
7. Upload the code to your ESP8266 device by clicking on the "Upload" button in the Arduino IDE.

### Usage

1. Once the code is uploaded successfully, open the Serial Monitor in the Arduino IDE to view the device's IP address.
2. Connect your computer or mobile device to the same Wi-Fi network as the ESP8266.
3. Open a web browser and enter the IP address of the ESP8266 in the address bar.
4. You will see a web interface where you can schedule jobs. Enter the cron schedule and URL for the job, then click "Save Job".
5. The job will be saved and scheduled for execution according to the provided cron schedule.
6. You can view the list of scheduled jobs by navigating to the "/joblist" URL.
7. The list will display the URL and the next scheduled runtime for each job.

## Customization

- You can modify the code to fit your specific needs. For example, you can add additional fields to the job data structure, customize the web interface, or extend the functionality to include more features.
- Make sure to follow the guidelines of the CronAlarms library when specifying cron schedules.
- If you need to store more jobs, you can adjust the `MAX_JOBS` constant in the code to a higher value. However, keep in mind the memory limitations of the ESP8266.

## Contributing

Contributions to this project are welcome! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

- This project utilizes the [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) library by Hristo Gochkov and the [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) library by Hristo Gochkov and Ayush Sharma.
- The job scheduling is handled by the [CronAlarms](https://github.com/esp8266/Arduino/tree/master/libraries/CronAlarms) library provided by the ESP8266 Arduino core.

## References

- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
- [Arduino IDE](https://www.arduino.cc/en/software)
