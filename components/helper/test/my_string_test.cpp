#include "../../my_coap/include/my_coap.h"
#include "../../wifi/include/wifi_station.h"
#include "print_system_info.h"
#include "print_coap_info.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "coap.h"

static const char* TAG = "APP MAIN";

static const char* string = "title=\"Rafael o cara\" rt=10 if=\"Obvio\"";

//void add_resource_attr(const char* str){
//	char *pEnd, *token;
//	char buffer[100];
//
//	strcpy(buffer, string);
//	token = strtok_r(buffer, " =", &pEnd);
//	if(token){
//		do{
//			printf("Token: %s\n", token);
//			token = strtok_r(pEnd, " =", &pEnd);
//		}while(token);
//	}
//}

#include "my_string.h"

static void my_string_substring(const char* string){
	My_String s(string);
	char buffer[100];
	int pos = 0;

	while((pos = s.substring(buffer, "= ", pos)) > 0){
		printf("[%d] %s\n", pos++, buffer);
	}
}

static void my_string_substring_console(const char* string){
	My_String s(string);
	char buffer[100];
	int pos = 0;

	while((pos = s.substring_console(buffer, "= ", pos)) > 0){
		printf("[%d] %s\n", pos++, buffer);
	}
}

static void compare_strings(){
	My_String s("num"), s2("num"), s3("num2");

	printf("My_String(num) ");
	if(s == "num"){
		printf("==");
	} else {
		printf("!=");
	}
	printf(" const char(num)\n");

	printf("My_String(num) ");
	if(s == "num2"){
		printf("==");
	} else {
		printf("!=");
	}
	printf(" const char(num2)\n");

	printf("My_String(num) ");
	if(s == s2){
		printf("==");
	} else {
		printf("!=");
	}
	printf(" My_String(num)\n");

	printf("My_String(num) ");
	if(s == s3){
		printf("==");
	} else {
		printf("!=");
	}
	printf(" My_String(num2)\n");
}

static void read_attributes(const char* str){
	My_String s(str);
	int pos = 0;
	char buffer[100], attr[50], value[50];

	while((pos = s.substring_console(buffer, " ", pos)) > 0){
		My_String s2(buffer);
		int pos2 = 0;

		pos2 = s2.substring_console(attr, "=", pos2);
		s2.substring_console(value, "", ++pos2);

		printf("%s = %s\n", attr, value);
		pos++;
	}
}

extern "C" void app_main(void)
{
	print_system_reset_reason();

	ESP_LOGI(TAG, "Iniciando!");

	printf("------substring--------\n");
	my_string_substring(string);
	printf("------substring_console--------\n");
	my_string_substring_console(string);
	printf("------compare string--------\n");
	compare_strings();
	printf("-------read attributes-------\n");
	read_attributes(string);
	printf("------------------\n");
	while(true){
		vTaskDelay(20000 / portTICK_PERIOD_MS);
	}
}
