// App2.ino
//

SYSTEM_MODE(MANUAL);

const uint8_t R200[] =
	"HTTP/1.0 200 OK\r\n"										\
	"Server: spark-core/1.0\r\n"						\
	"Content-Type: text/html\r\n"						\
	"Connection: close\r\n"									\
	"\r\n"																	\
	"<html><pre>OK<br />[%s]</pre></html>";

const uint8_t R404[] =
	"HTTP/1.0 404 Not Found\r\n"						\
	"Server: spark-core/1.0\r\n"						\
	"Content-Type: text/html\r\n"						\
	"Connection: close\r\n"									\
	"\r\n";

const uint8_t cmd1[] = "GET / HTTP/1.1\r\n";
const uint8_t cmd2[] = "GET /value1 HTTP/1.1\r\n";
const uint8_t cmd3[] = "POST /reset HTTP/1.1\r\n";

TCPServer server = TCPServer(12345);
TCPClient client;

#define BUFSIZE			1024

uint32_t c, t;
uint8_t ibuf[BUFSIZE], obuf[BUFSIZE];

void setup() {
	Serial.begin(115200);
//	while (!Serial.available());

	WiFi.connect();
	while (!WiFi.ready());

	server.begin();

	Serial.println("READY");
}

void loop() {
	SPARK_WLAN_Loop();

	if (client.connected()) {
		c = t = 0;
		while (client.available() > 0) {
			SPARK_WLAN_Loop();
			c = client.read(&ibuf[t], sizeof(ibuf) - t);
			t += c;
		}

		ibuf[t] = 0;
		SPARK_WLAN_Loop();

		if (t > 0 && strlen((const char *)ibuf) > 0)
		{
			uint8_t tmp[1024];
			sprintf((char *)tmp, "[%s]", (const char *)ibuf);
			Serial.println((const char *)tmp);

			if (strncmp((const char *)ibuf, (const char *)cmd1, sizeof(cmd1) - 1) == 0)
				sprintf((char *)obuf, (const char *)R200, (const char *)ibuf);
			else if (strncmp((const char *)ibuf, (const char *)cmd2, sizeof(cmd2) - 1) == 0)
				sprintf((char *)obuf, (const char *)R200, "Some value...");
			else if (strncmp((const char *)ibuf, (const char *)cmd3, sizeof(cmd3) - 1) == 0)
				NVIC_SystemReset();
			else
				sprintf((char *)obuf, (const char *)R404);

			client.write(obuf, strlen((const char *)obuf));

			SPARK_WLAN_Loop();
		}

		client.stop();
	}
	else {
		client = server.available();
	}
}
