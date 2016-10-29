/***************************************
 * INTERPRETER.cpp
 *
 * Student Name: YOUR NAME
 * Student ID: YOUR 8-DIGIT ID
 * UCI Net ID: YOUR UCI-NET ID
 **************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cstring>

#define DATA_SEG_SIZE 100

class INTERPRETER {
public:
  INTERPRETER(char *sourceFile);
  void runProgram();

private:
  std::ifstream codeIn;
  std::ifstream inputIn;
  std::ofstream outFile;

  std::vector<std::string> C;
  int D[DATA_SEG_SIZE];
  int PC;
  std::string IR;
  bool run_bit;

  unsigned curIRIndex;

  // my variables

  int dataIndex; 
  int number;
  bool dataFlag = false;
  bool writeFlag = false;  
  int recursionCounter = 0; 
  int total = 0; 
  bool adderMux = false; 
  bool adderFlag = false; 
  int factor = 0; 
  bool mult = false; 
  bool division = false; 
  bool modulo = false;
  int jumpDestination = 0; 
  int left = 0; 
  int right = 0; 
  std::string op = ""; 

  void printDataSeg();

  void fetch();
  void incrementPC();
  void execute();

  // Output: used in the case of: set write, source
  void write(int source);

  // Input: used in the case of: set destination, read
  int read();

  /**
   * Checks and returns if the character c is found at the current
   * position in IR. If c is found, advance to the next
   * (non-whitespace) character.
   */
  bool accept(char c);

  /**
   * Checks and returns if the string s is found at the current
   * position in IR. If s is found, advance to the next
   * (non-whitespace) character.
   */
  bool accept(const char *s);

  /**
   * Checks if the character c is found at the current position in
   * IR. Throws a syntax error if c is not found at the current
   * position.
   */
  void expect(char c);

  /**
   * Checks if the string s is found at the current position in
   * IR. Throws a syntax error if s is not found at the current
   * position.
   */
  void expect(const char *s);

  void skipWhitespace();

  void parseStatement();
  void parseSet();
  void parseJump();
  void parseJumpt();
  void parseExpr();
  void parseTerm();
  void parseFactor();
  void parseNumber();

  void syntaxError();

  // my functions 

  void halt();
  void processNumber();
  void handleRecursion();
  void assign();
  void destination();
  void handleAddition();
  void handleSubtraction();
  void handleMultiplication();
  void handleDivision();
  void handleModulo();
  void processJump(); 
  void processJumpt();
  void assignOperator(const char *s);

};


INTERPRETER::INTERPRETER(char *sourceFile) {
  codeIn.open(sourceFile, std::fstream::in);
  if (codeIn.fail()) {
    std::cerr << "init: Errors accessing source file "
              << sourceFile << std::endl;
    exit(-2);
  }

  inputIn.open("input.txt", std::fstream::in);
  if (inputIn.fail()) {
    std::cerr << "init: Errors accessing input file input.txt" << std::endl;
    exit(-2);
  }

  outFile.open((std::string(sourceFile) + ".out").c_str(), std::fstream::out);
  if (outFile.fail()) {
    std::cerr << "init: Errors accessing output file "
              << std::string(sourceFile) + ".out" << std::endl;
    exit(-2);
  }

  // Initialize the SIMPLESEM processor state
  // Initialize the Code segment
  while (codeIn.good()) {
    std::string line;
    std::getline(codeIn, line);
    C.push_back(line);
  }

  // Initialize the Data segment
  for (int i=0; i<DATA_SEG_SIZE ;i++) {
    D[i]=0;
  }
  PC = 0; // Every SIMPLESEM program begins at instruction 0
  run_bit = true; // Enable the processor
}

void INTERPRETER::runProgram() {

  // TODO FETCH-INCREMENT-EXECUTE CYCLE

  while (run_bit) {
    fetch();
    incrementPC();
    execute();
  }

  printDataSeg();
}

void INTERPRETER::printDataSeg() {
  outFile << "Data Segment Contents" << std::endl;
  for(int i=0; i < DATA_SEG_SIZE; i++){
    outFile << i << ": " << D[i] << std::endl;
  }
}

void INTERPRETER::fetch() {
  // TODO
  // The IR is updated with the instruction pointed at by PC; IR=C[PC]
  IR = C.at(PC);
  curIRIndex = 0; 
}

void INTERPRETER::incrementPC() {
  // TODO
  // The PC is incremented to *point* to the next instruction in C; PC = PC+1
  ++PC;
}

void INTERPRETER::execute() {
  // TODO
  parseStatement(); 
}

//Output: used in the case of: set write, source
void INTERPRETER::write(int source){
  outFile << source << std::endl;
}

//Input: used in the case of: set destination, read
int INTERPRETER::read() {
  int value;
  inputIn >> value;
  return value;
}

/**
 * Checks and returns if the character c is found at the current
 * position in IR. If c is found, advance to the next
 * (non-whitespace) character.
 */
bool INTERPRETER::accept(char c) {
  if (curIRIndex >= IR.length())
    return false;

  if (IR[curIRIndex] == c) {
    curIRIndex++;
    skipWhitespace();
    if (c == '+') {
      handleAddition();
    } else if (c == '-') {
      handleSubtraction();
    } else if (c == '*') {
      handleMultiplication();
    } else if (c == '/') {
      handleDivision();
    } else if (c == '%') {
      handleModulo();
    } else if (c == '<') {
      op = "<";
    } else if (c == '>') {
      op = ">";
    }
    return true;
  } else {
    return false;
  }
}

/**
 * Checks and returns if the string s is found at the current
 * position in IR. If s is found, advance to the next
 * (non-whitespace) character.
 */
bool INTERPRETER::accept(const char *s) {
  unsigned s_len = strlen(s);

  if (curIRIndex+s_len > IR.length())
    return false;

  for (unsigned i = 0; i < s_len; ++i) {
    if (IR[curIRIndex+i] != s[i]) {
      return false;
    }
  }

  curIRIndex += s_len;
  skipWhitespace();
  assignOperator(s);
  return true;
}


/**
 * Checks if the character c is found at the current position in
 * IR. Throws a syntax error if c is not found at the current
 * position.
 */
void INTERPRETER::expect(char c) {
  if (!accept(c))
    syntaxError();
}

/**
 * Checks if the string s is found at the current position in
 * IR. Throws a syntax error if s is not found at the current
 * position.
 */
void INTERPRETER::expect(const char *s) {
  if (!accept(s))
    syntaxError();
}

void INTERPRETER::skipWhitespace() {
  while (curIRIndex < IR.length() && isspace(IR[curIRIndex]))
    curIRIndex++;
}

void INTERPRETER::parseStatement() {
  std::cerr << "Statement" << std::endl;

  if (accept("halt")) {
    halt();
    return;
  }
  else if(accept("set"))
    parseSet();
  else if(accept("jumpt")) {
    parseJumpt();
    processJumpt();
  }
  else if(accept("jump")) {
    parseJump();
    processJump();
  }
}

void INTERPRETER::parseSet() {
  std::cerr << "Set" << std::endl; 

  if (!accept("write")) {
    parseExpr();
    dataIndex = number; 
  } else {
    writeFlag = true; 
  }

  expect(',');

  if (!accept("read")) { 
    parseExpr();
  } else { 
    number = INTERPRETER::read();
  }

  assign();
}

void INTERPRETER::parseExpr() {
  std::cerr << "Expr" << std::endl;

  parseTerm();

  while (accept('+') || accept('-')) {
    total = number; 

    parseTerm();
    
    if (adderMux)
      total += number;
    else
      total -= number;

    number = total; 
  } 
}

void INTERPRETER::parseTerm() {
  std::cerr << "Term" << std::endl;

  parseFactor();

  while (accept('*') || accept('/') || accept('%')) {
    factor = number; 

    parseFactor();

    if (mult) {
      factor *= number; 
      mult = false; 
    } else if (division) {
      factor /= number;
      division = false; 
    } else if (modulo) {
      factor %= number; 
      modulo = false; 
    }

    number = factor; 
  }
}

void INTERPRETER::parseFactor() {
  std::cerr << "Factor" << std::endl;

  if (accept("D[")) {

    ++recursionCounter; 

    parseExpr();

    expect(']');

    handleRecursion();

  } else if (accept('(')) {
    parseExpr();

    expect(')');
    
  } else {
    parseNumber();
  }
}

void INTERPRETER::parseNumber() {
  std::cerr << "Number" << std::endl;

  if (curIRIndex >= IR.length())
    syntaxError();

  if (IR[curIRIndex] == '0') {
    number = 0; 

    curIRIndex++;
    skipWhitespace();

    return;
  } else if (isdigit(IR[curIRIndex])) {
    int pos = curIRIndex;
    int len = 0; 
    while (curIRIndex < IR.length() &&
           isdigit(IR[curIRIndex])) {
      ++len;
      curIRIndex++;
    }
    
    number = std::stoi(IR.substr(pos,len));

    skipWhitespace();
  } else {
    syntaxError();
  } 
}

void INTERPRETER::parseJump() {
  std::cerr << "Jump" << std::endl;

  parseExpr();
}

//<Jumpt>-> jumpt <Expr>, <Expr> ( != | == | > | < | >= | <= ) <Expr>
void INTERPRETER::parseJumpt() {
  std::cerr << "Jumpt" << std::endl;

  parseExpr();

  jumpDestination = number;  
 
  expect(',');

  parseExpr();

  left = number;  

  if (accept("!=") || accept("==") || accept(">=") || accept("<=") ||
      accept('>') || accept('<')) {
    parseExpr();
    right = number; 
  } else {
    syntaxError();
  }
}

void INTERPRETER::syntaxError() {
  std::cerr << "Syntax Error!" << std::endl;
  exit(-1);
}

void INTERPRETER::halt() {
  run_bit = false; 
}

void INTERPRETER::destination() {
  if (dataFlag) {
    dataIndex = number;
    dataFlag = false; 
  }
}

void INTERPRETER::assign() { 
  if (writeFlag) { 
    write(number);
    writeFlag = false; 
  } 
  else {
    D[dataIndex] = number;
  }
}

void INTERPRETER::handleRecursion() {
  for (int i = 0; i < recursionCounter; i++)
    number = D[number];

  recursionCounter = 0; 
}

void INTERPRETER::handleAddition() {
  adderMux = true; 
}

void  INTERPRETER::handleSubtraction() {
  adderMux = false;  
}

void INTERPRETER::handleMultiplication() {
  mult = true; 
}
void INTERPRETER::handleDivision() {
  division = true; 
}
void INTERPRETER::handleModulo() {
  modulo = true; 
}

// ****** Jump

void INTERPRETER::processJump() {
  PC = number; 
}


// ****** Jumpt

void INTERPRETER::processJumpt() {
  if (op == "==") {
    if (left == right) {
      PC = jumpDestination; 
    }
  } else if (op == "!=") {
    if (left != right) {
      PC = jumpDestination; 
    }
  } else if (op == ">") {
    if (left > right) {
      PC = jumpDestination; 
    }    
  } else if (op == "<") {
     if (left < right) {
      PC = jumpDestination; 
    }   
  } else if (op == ">=") {
    if (left >= right) {
      PC = jumpDestination; 
    }  
  } else if (op == "<=") {
    if (left <= right) {
      PC = jumpDestination; 
    }
  }
}

void INTERPRETER::assignOperator(const char *s) {
  if (s == "==")
    op = "==";
  else if (s == "!=")
    op = "!=";
  else if (s == "<")
    op = "<";
  else if (s == ">")
    op = ">";
  else if (s == "<=")
    op = "<=";
  else if (s == ">=")
    op = ">=";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "init: This program requires an input "
              << "filename as a parameter " << std::endl;
    exit(-2);
  }
  INTERPRETER i(argv[1]);
  i.runProgram();
}

