#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "nyu"
#define WIFI_PASSWORD "P4SSW0RDxyzA"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "testcodegx@gmail.com"
#define AUTHOR_PASSWORD ""

#define RECIPIENT_EMAIL "flamingburrito1628@gmail.com"


SMTPSession smtp;
SMTP_Message universalMessage;

void smtpCallback(SMTP_Status status);

bool sentEmail = false;
int pin1 = 18;
int pin2 = 19;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, pin2, pin1);
  Serial.println();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  MailClient.networkReconnect(true);

  smtp.debug(1);//change to 0 if not debugging
  smtp.callback(smtpCallback);

  Session_Config config;
  //smtp session configuration
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = -5;
  config.time.day_light_offset = 0;

  //messge setup
  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = F("ESP");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("ESP Test Email");
  message.addRecipient(F("Test"), RECIPIENT_EMAIL);


  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  if (!MailClient.sendMail(&smtp, &message)){
  Serial.println("Error sending Email, " + smtp.errorReason());
  }
}

void sendEmail(SMTP_Message imessage){
  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &imessage)){
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
  }
}

void setup_method(SMTP_Message input){
  
  /* Set the message headers */
  input.sender.name = F("ESP");
  input.sender.email = AUTHOR_EMAIL;
  input.subject = F("ESP Test Email");
  input.addRecipient(F("Test"), RECIPIENT_EMAIL);


  String textMsg = "Hello World! - Sent from ESP board";
  input.text.content = textMsg.c_str();
  input.text.charSet = "us-ascii";
  input.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  input.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  input.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

}

void loop() {
  setup_method(universalMessage);
    bool inByte = Serial1.read();
    if(!sentEmail && inByte){
      sendEmail(universalMessage);
      sentEmail = true;
    }
}



/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
      
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}
