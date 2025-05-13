#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// WiFi Credentials
const char* ssid;
printf("Enter your ssid: ");
scanf("%s", &ssid);

const char* password;
printf("Enter your password: ");
scanf("%s", &password);

// MQTT Broker IP (your GCP instance)
const char* mqtt_server;
printf("Enter your Broker IP: ");
scanf("%s", &mqtt_server);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi. Restarting...");
    ESP.restart();
  }
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" Connected!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received: ");
  Serial.println(message);

  // Parse move string: "X:1,2"
  if (String(topic) == "tictactoe/player_move") {
    currentPlayer = message.charAt(0);
    moveRow = message.charAt(2) - '0';
    moveCol = message.charAt(4) - '0';
    moveReceived = true;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected!");
      client.subscribe("tictactoe/player_move");
    } else {
      Serial.print("failed, retrying in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  randomSeed(esp_random());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

char board[3][3];
int turn = 0;
int mode = 0;
void setup_wifi() {
  delay(10);
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" Connected!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received: ");
  Serial.println(message);

  // Parse move string: "X:1,2"
  if (String(topic) == "tictactoe/player_move") {
    currentPlayer = message.charAt(0);
    moveRow = message.charAt(2) - '0';
    moveCol = message.charAt(4) - '0';
    moveReceived = true;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected!");
      client.subscribe("tictactoe/player_move");
    } else {
      Serial.print("failed, retrying in 5s");
      delay(5000);
    }
  }
}

void reset_board() {
    int k = '1';
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = k++;
}

void print_board() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf(" %c | %c | %c \n", board[i][0], board[i][1], board[i][2]);
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}

int check_winner() {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) return 1;
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) return 1;
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) return 1;
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) return 1;
    return 0;
}

int is_full() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 'X' && board[i][j] != 'O') return 0;
    return 1;
}

int get_move_from_command(const char *cmd) {
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("Failed to run command");
        return -1;
    }

    char buf[32];
    if (fgets(buf, sizeof(buf), fp) != NULL) {
        int move = atoi(buf);
        pclose(fp);
        return move;
    }
    pclose(fp);
    return -1;
}

int get_human_move() {
    int move;
    while (1) {
        printf("Player %c, enter move (1-9): ", (turn == 0) ? 'X' : 'O');
        if (scanf("%d", &move) == 1 && move >= 1 && move <= 9) {
            int row = (move - 1) / 3;
            int col = (move - 1) % 3;
            if (board[row][col] != 'X' && board[row][col] != 'O') {
                return move;
            } else {
                printf("Cell is taken. Try again.\n");
            }
        } else {
            printf("Invalid input.\n");
            while (getchar() != '\n');
        }
    }
}

int make_move(int move) {
    int row = (move - 1) / 3;
    int col = (move - 1) % 3;

    if (move < 1 || move > 9 || board[row][col] == 'X' || board[row][col] == 'O') {
        return 0;
    }

    board[row][col] = (turn == 0) ? 'X' : 'O';
    print_board();

    if (check_winner()) {
        printf("Player %c wins!\n", (turn == 0) ? 'X' : 'O');
        return 2;
    }

    if (is_full()) {
        printf("It's a draw!\n");
        return 3;
    }

    turn = 1 - turn;
    return 1;
}

void show_menu() {
    printf("=== TIC-TAC-TOE MENU ===\n");
    printf("1. One-player mode (Human vs Bash Script)\n");
    printf("2. Two-player mode (Human vs Human)\n");
    printf("3. Automated mode (C Bot vs Bash Script)\n");
    printf("Select mode (1, 2 or 3): ");
    while (scanf("%d", &mode) != 1 || mode < 1 || mode > 3) {
        printf("Invalid input. Try again: ");
        while (getchar() != '\n');
    }
}

int main() {
    show_menu();
    reset_board();
    print_board();

    while (1) {
        int move = -1;

        if (mode == 1) { // Human vs Bash
            if (turn == 0) {
                move = get_human_move();
            } else {
                printf("Calling Bash script for Player O...\n");
                move = get_move_from_command("./player2_script.sh");
            }
        } else if (mode == 2) { // Human vs Human
            move = get_human_move();
        } else if (mode == 3) { // Automated C vs Bash
            if (turn == 0) {
                printf("Calling C bot for Player X...\n");
                move = get_move_from_command("./player1_bot");
            } else {
                printf("Calling Bash script for Player O...\n");
                move = get_move_from_command("./player2_script.sh");
            }
        }

        printf("Player %c selected move: %d\n", (turn == 0) ? 'X' : 'O', move);

        int result = make_move(move);
        if (result == 0) {
            printf("Invalid move. Skipping turn.\n");
        } else if (result >= 2) {
            break;
        }
    }

    return 0;
}
