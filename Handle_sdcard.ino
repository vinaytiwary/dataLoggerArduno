/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
 #include "Functions.h"
#include "EEPROM.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "common.h"
#include "ESP32Time.h"
#include "time.h"
#include "debug.h"
char buffer[14];
ESP32Time rtc;
static uint64_t log_no=0;
extern ram_data_t ram_data;

extern e2p_date_t e2p_date;
File logFile;
SPIClass * vspi = NULL;
//spi->begin(ESP_SCK, ESP_MISO, ESP_MOSI, ESP_SD_CS);
char * log_col_names[LOG_COLUMN_COUNT] = 
{
  "log no.","Date", "Time", "latitude", "longitude", "altitude", "speed",  "PDOP"
}; 
 File file = SD.open("/test1.csv", FILE_WRITE);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    SerialMon.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        SerialMon.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        SerialMon.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            SerialMon.print("  DIR : ");
            SerialMon.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            SerialMon.print("  FILE: ");
            SerialMon.print(file.name());
            SerialMon.print("  SIZE: ");
            SerialMon.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    SerialMon.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        SerialMon.println("Dir created");
    } else {
        SerialMon.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    SerialMon.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        SerialMon.println("Dir removed");
    } else {
        SerialMon.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    SerialMon.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        SerialMon.println("Failed to open file for reading");
        return;
    }

    SerialMon.print("Read from file: ");
    while(file.available()){
        SerialMon.write(file.read());
    }
    file.close();
}

void readFile_testing(fs::FS &fs, const char * path){
    SerialMon.printf("Reading file testing: %s\n", path);

    File file = fs.open(path);
    if(!file){
        SerialMon.println("Failed to open file for reading testing");
        return;
    }

    SerialMon.print("Read from file testing: ");
    while(file.available()){
        SerialMon.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    SerialMon.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        SerialMon.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        SerialMon.println("File written");
    } else {
        SerialMon.println("Write failed");
    }
    file.close();
}

void writeFile_testing(fs::FS &fs, const char * path, const char * message){
    SerialMon.printf("Writing file testing: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        SerialMon.println("Failed to open file for writing_testing");
        return;
    }
    if(file.print(message)){
        SerialMon.println("File written testing");
    } else {
        SerialMon.println("Write failed testing");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    SerialMon.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        SerialMon.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        SerialMon.println("Message appended");
    } else {
        SerialMon.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    SerialMon.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        SerialMon.println("File renamed");
    } else {
        SerialMon.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    SerialMon.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        SerialMon.println("File deleted");
    } else {
        SerialMon.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        SerialMon.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        SerialMon.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        SerialMon.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    SerialMon.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void init_sdcard()
{
	//SPI.begin(ESP_SCK,ESP_MISO,ESP_MOSI,ESP_SD_CS);
	
	vspi = new SPIClass(VSPI);
	vspi->begin();
	vspi->begin(ESP_SCK, ESP_MISO, ESP_MOSI, ESP_SD_CS);
	pinMode(ESP_SD_CS, OUTPUT); //HSPI SS
	digitalWrite(ESP_SD_CS,HIGH);
    if(!SD.begin(ESP_SD_CS, *vspi))
	{
        SerialMon.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        SerialMon.println("No SD card attached");
        return;
    }

    SerialMon.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        SerialMon.println("MMC");
    } else if(cardType == CARD_SD){
        SerialMon.println("SDSC");
    } else if(cardType == CARD_SDHC){
        SerialMon.println("SDHC");
    } else {
        SerialMon.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    SerialMon.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    writeFile_testing(SD, "/vinay.txt", "Swagat hai aapka! ");
    readFile_testing(SD, "/foo.txt");
    readFile_testing(SD, "/vinay.txt");
    readFile_testing(SD, "/gps.csv");
    testFileIO(SD, "/test.txt");
    printHeader();
    SerialMon.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    SerialMon.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void update_sd_card()               //Vinay 21-07-2023
{
#ifdef DEBUG_RTC
    Serial.print("gettime=");
    Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
#endif

    //check_RTC_time();
    //utcTOlocal();
    if(check_RTC_time() ==incorrect)
    {
      if(gps_data.GPS_flag ==true)
      {
#ifdef DEBUG_GPS_DATA
        Serial.print("G.Y=");
        Serial.println(gps_data.gps_date_time.year);
        Serial.print("G.M=");
        Serial.println(gps_data.gps_date_time.month);
        Serial.print("G.D=");
        Serial.println(gps_data.gps_date_time.day);
        Serial.print("G.H=");
        Serial.println(gps_data.gps_date_time.hour);
        Serial.print("G.mn=");
        Serial.println(gps_data.gps_date_time.min);
        Serial.print("G.S=");
        Serial.println(gps_data.gps_date_time.sec);
#endif       
        rtc.setTime(gps_data.gps_date_time.sec,  gps_data.gps_date_time.min,gps_data.gps_date_time.hour,gps_data.gps_date_time.day,gps_data.gps_date_time.month,gps_data.gps_date_time.year+2000);

#ifdef DEBUG_RTC
        Serial.print("year=");
        Serial.println(rtc.getYear());
#endif

      }
    }
    else
    {
       date_change_e2p();
    }
    
#ifdef DEBUG_RTC
    Serial.printf("\nRTC_HH = %d", rtc.getHour(true));
    Serial.printf("\nRTC_QQ = %d", (rtc.getHour(true)/6)+1);
#endif
    
    ram_data.time_stamp.yy = (rtc.getYear())%100;
    ram_data.time_stamp.mm = rtc.getMonth()+1;
    ram_data.time_stamp.dd = rtc.getDay();
    ram_data.time_stamp.hr = rtc.getHour(true);
    ram_data.time_stamp.min = rtc.getMinute();
    ram_data.time_stamp.sec = rtc.getSecond();
#ifdef DEBUG_RAM_DATA
    Serial.print("R.Y=");
    Serial.println(ram_data.time_stamp.yy);
    Serial.print("R.M=");
    Serial.println(ram_data.time_stamp.mm);
    Serial.print("R.D=");
    Serial.println(ram_data.time_stamp.dd);
    Serial.print("R.H=");
    Serial.println(ram_data.time_stamp.hr);
    Serial.print("R.mn=");
    Serial.println(ram_data.time_stamp.min);
    Serial.print("R.S=");
    Serial.println(ram_data.time_stamp.sec);
#endif
    //check_RTC_time();
    /*
    int sec=rtc.getSecond();
    int mnt =rtc.getMinute();
    int hours=rtc.getHour(true);
    */
    log_no++;
    //SerialMon.println("update");
    if(SD.exists(buffer))
    {
      
#ifdef DEBUG_SD_CARD
      Serial.println("File Exist..");
#endif

      logFile  = SD.open(buffer, FILE_APPEND);
      //if(logFile)
      //{
      logFile.print(log_no);
      logFile.print(",");
      //SerialMon.println("update__data");
      logFile.printf("%02d/%02d/20%02d",ram_data.time_stamp.dd,ram_data.time_stamp.mm,ram_data.time_stamp.yy);
      logFile.print(",");
      logFile.printf("%02d:%02d:%02d",ram_data.time_stamp.hr,ram_data.time_stamp.min,ram_data.time_stamp.sec);//rtc.getHour(true),rtc.getMinute(),rtc.getSecond);
      logFile.print(",");
      //logFile.printf("%d.%06d",ram_data.lat/100000,ram_data.lat%100000);
      logFile.printf("%02ld.%06ld",ram_data.lat/1000000L,ram_data.lat%1000000L);
      logFile.print(",");
      //logFile.printf("%d.%06d",ram_data.lon/100000,ram_data.lon%100000);
      logFile.printf("%02ld.%06ld",ram_data.lon/1000000L,ram_data.lon%1000000L);
      logFile.print(",");
      //logFile.printf("%d.%02d",ram_data.alt/100000,ram_data.alt%100000);
      logFile.printf("%03d.%d",ram_data.alt/10,ram_data.alt%10);
      logFile.print(",");
      //logFile.print(ram_data.speed);
      logFile.printf("%d.%d",ram_data.speed/10,ram_data.speed%10);
      logFile.print(",");
      //logFile.printf("%d.%02d",ram_data.accuracy/100000,ram_data.accuracy%100000);
      logFile.printf("%d.%d",ram_data.accuracy/10,ram_data.accuracy%10);
      logFile.print("\n");
      /*
      logFile.print(log_no);
      logFile.print(",");
      logFile.print(ram_data.year); //Print the first data string onto the MicroSD Card
      logFile.print(",");
      logFile.print(ram_data.month);
      logFile.print(",");
      logFile.print(ram_data.day);
      logFile.print(",");
      logFile.print(ram_data.hour);
      logFile.print(",");
      logFile.print(ram_data.min);
      logFile.print(",");
      logFile.print(ram_data.sec);
      logFile.print(",");
      logFile.print(ram_data.lat);
      logFile.print(",");
      logFile.print(ram_data.lon);
      logFile.print(",");
      logFile.print(ram_data.alt);
      logFile.print(",");
      logFile.print(ram_data.speed);
      logFile.print(",");
      logFile.print(ram_data.accuracy);
      logFile.print("\n");
      */
      //}
      logFile.close();
    }
    else
    {
      log_no=0;
      printHeader();
    }
    
}
void printHeader()              //Vinay 21-07-2023
{
  logFile = SD.open(buffer, FILE_WRITE); // Open the log file
  
  if (logFile) // If the log file opened, print our column names to the file
  {
    int i = 0;
    for (; i < LOG_COLUMN_COUNT; i++)
    {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1) // If it's anything but the last column
        logFile.print(','); // print a comma
      else // If it's the last column
        logFile.println(); // print a new line
    }
    logFile.close(); // close the file
  }
}
void prepare_data()   //Vinay 29-07-2023
{
  //vspi = new SPIClass(VSPI);
  //vspi->begin();
  //vspi->begin(ESP_SCK, ESP_MISO, ESP_MOSI, ESP_SD_CS);
  pinMode(ESP_SD_CS, OUTPUT); //HSPI SS
  digitalWrite(ESP_SD_CS,HIGH);
  /*
  time_main.yy =rtc.getYear();
  time_main.mm =rtc.getMonth();
  time_main.dd =rtc.getDay();
  time_main.hr =rtc.getHour(true);
  time_main.min =rtc.getMinute();
  time_main.sec =rtc.getSecond();
  check_RTC_time();
  Serial.println("preparing Data....");
  Serial.print("A=");
  Serial.println(time_main.min);
  Serial.print("B=");
  Serial.print(rtc.getMinute());
  */
  //char buffer[130];
  //sprintf(buffer,"%d,%d,%d,%d,%d,%d,%lu,%lu,%lu,%lu",ram_data.year,ram_data.month,ram_data.day,ram_data.hour,ram_data.min,ram_data.sec,ram_data.lat,ram_data.lon,ram_data.speed,ram_data.accuracy);
 
    //uint8_t time_quater=0;
    ///time_quater = ((ram_data.time_stamp.hr/6)+1);
    
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"/%02d%02d%02d_%02d.csv",e2p_date.yy,e2p_date.mm,e2p_date.dd,e2p_date.QQ);
#ifdef DEBUG_EEPROM
    Serial.print("e2p.yy=");
    Serial.println(e2p_date.yy);
    Serial.print("e2p.mm=");
    Serial.println(e2p_date.mm);
    Serial.print("e2p.dd=");
    Serial.println(e2p_date.dd);
    Serial.print("e2p.qq=");
    Serial.println(e2p_date.QQ);
    Serial.print("file name=");
    Serial.println(buffer);
#endif
}

//void read_eeprom()
//{
 // EEPROM.get(E2P_ADD_FILE_NAME, buffer);
 // Serial.print("read buffer=");
 // Serial.println(buffer);
  /*
  EEPROM.get(E2P_ADD_FILE_NAME, e2p_date);
  memcpy(time_main,e2p_date,sizeof(e2p_date));
  Serial.print("read buffer=");
  Serial.println(buffer);
  */
//}
void set_rtc_time()          //Vinay 31-07-2023
{
  rtc.setTime(00, 00, 13, 01, 01, 2000);
  
#ifdef DEBUG_RTC
  Serial.print("setup_time");
  Serial.println(rtc.getDateTime());
#endif

}
