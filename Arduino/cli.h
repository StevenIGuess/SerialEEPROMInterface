//From stackoverflow

#define CLI_CMD_NUM_MAX 6 // Max size of command list
#define CLI_BUFFER_SIZE 32 // Max length of the serial command buffer
#define CLI_MODE_WAITING_START 0
#define CLI_MODE_READING 1
#define CLI_MODE_WAITING_EOL 2

class MyCli {
  public:
    MyCli();
    void reset();
    void think();
    void execLine();
    void printHelp();
    void addCmd(char *cmd, void (*callback)(char*, char*));
    byte cliMode;
  private:
    char buffer[CLI_BUFFER_SIZE];
    byte bufferIdx;
    void bufferReset();
    
    //MyCliCmd* cmdList[CLI_CMD_NUM_MAX];
    byte cmdListIdx;
    char* cmdListCmd[CLI_CMD_NUM_MAX];
    void (*cmdListCallback[CLI_CMD_NUM_MAX])(char*, char*);
};
MyCli::MyCli() {
  cliMode = CLI_MODE_WAITING_START;
  memset(buffer, 0, CLI_BUFFER_SIZE);
  
  cmdListIdx = 0;
  memset(cmdListCmd, 0, sizeof(cmdListCmd));
  memset(cmdListCallback, 0, sizeof(cmdListCallback));
  bufferReset();
}

void MyCli::addCmd(char* cmd, void (*_callback)(char*, char*)) {
  // Don't add a CMD if the list is full
  if (cmdListIdx >= CLI_CMD_NUM_MAX) {
    return;
  }
  // Make commands upper case, to ignore case sensitivity
  strupr(cmd);
  cmdListCmd[cmdListIdx] = cmd;
  cmdListCallback[cmdListIdx] = _callback;
  cmdListIdx++;
}
void MyCli::think() {
  while (Serial.available()) {
    char c = Serial.read();
    if (cliMode == CLI_MODE_WAITING_START) {
      if (c >= 32 && c != ' ') {
        cliMode = CLI_MODE_READING; 
      }
    }
    if (cliMode == CLI_MODE_WAITING_EOL) {
      if (c == '\n' || c == '\r') {
        cliMode = CLI_MODE_WAITING_START;
        return;
      }
    }
    if (cliMode == CLI_MODE_READING) {
      if (c == '\n' || c == '\r') {

        this->execLine();
        return;
      }
      // Add the character to the buffer
      if (bufferIdx < CLI_BUFFER_SIZE) {
        buffer[bufferIdx] = c;
        bufferIdx++;
        // If the buffer is full
        if (bufferIdx >= CLI_BUFFER_SIZE-1) {
          // Overflow
          cliMode = CLI_MODE_WAITING_EOL;
          return;
        }
      }
    }
  }
}
// Execute the CLI command
void MyCli::execLine() {
  if (bufferIdx == 0) {
    return; 
  }
  // Get the first part, ready to the first space character
  char delim[] = " \r\n";
  char* cliPartPtr;
  char* cliPart = strtok_r(buffer, delim, &cliPartPtr);
  strupr(cliPart); // Make uppercase, so command is not case sensitive. "set" and "SeT" is the same as "SET".
  // Loop through commands, looking for one with a matching command.
  // Once found, run it
  bool ranCmd = false;
  if (!strcmp("HELP", cliPart)) {
    ranCmd = true;
  }
  if (!ranCmd) {
    for (byte iCmd=0; iCmd < cmdListIdx; iCmd++) {
      if (!strcmp(cmdListCmd[iCmd], cliPart)) {
        cmdListCallback[iCmd](cliPart, cliPartPtr);
        ranCmd = true;
      }
    }
  }
  
  
  // Cleanup to get ready for the next line
  this->reset();
}
void MyCli::reset() {
  cliMode = CLI_MODE_WAITING_START;
  bufferReset();
}
void MyCli::bufferReset() {
  bufferIdx = 0;
  memset(buffer, 0, CLI_BUFFER_SIZE);
}