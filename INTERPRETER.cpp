/***************************************
 * INTERPRETER.cpp
 *
 * Student Name: Francisco Franco
 * Student ID: 89265601
 * UCI Net ID: frfranco
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
  std::vector<std::string> Arguments; 
	int D[DATA_SEG_SIZE];
	int PC;
  std::string IR;
	bool run_bit;

  unsigned curIRIndex;

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

  // ** this is how you read from a vector ** 
  // for (std::vector<std::string>::const_iterator i = C.begin(); i != C.end(); ++i)
  //   std::cout << *i << std::endl;

  // Initialize the Data segment
  for (int i=0; i<DATA_SEG_SIZE ;i++) {
    D[i]=0;
  }
  PC = 0; // Every SIMPLESEM program begins at instruction 0
  run_bit = true; // Enable the processor
}

void INTERPRETER::runProgram() {
  std::cout << "inside runProgram()" << std::endl; 
  // TODO FETCH-INCREMENT-EXECUTE CYCLE
 
  // while (run_bit) {
  //     fetch();
  //     std::cout << IR << std::endl; 
  //     std::cout << PC << std::endl; 
  //     incrementPC();
  //     std::cout << PC << std::endl; 
  //     execute();
  // }

  fetch();
  // std::cout << IR << std::endl; 
  // std::cout << PC << std::endl; 
  incrementPC();
  // std::cout << PC << std::endl; 
  execute();

  std::cout << std::endl; 
  // TODO FETCH-INCREMENT-EXECUTE CYCLE

  printDataSeg();
}

void INTERPRETER::printDataSeg() {
  std::cout << "inside printDataSeg()" << std::endl;

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
  // std::cout << IR << std::endl; 
  parseStatement();
  // std::cout << "inside execute()" << std::endl; 
  // std::cout << IR << std::endl; 
  if (IR == "halt") {
    run_bit = false; 
  }
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
    std::cout << c;
    std::cout << " "; 
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

  // std::cout << s ; 
  unsigned s_len = strlen(s);
  // std::cout << s_len ; 

  if (curIRIndex+s_len > IR.length())
    return false;

  for (unsigned i = 0; i < s_len; ++i) {
    if (IR[curIRIndex+i] != s[i]) {
      return false;
    }
  }

  curIRIndex += s_len;
  skipWhitespace();

  std::cout << s;
  std::cout << " ";
  
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

  if (accept("halt"))
    return;
  else if(accept("set"))
    parseSet();
  else if(accept("jumpt"))
    parseJumpt();
  else if(accept("jump"))
    parseJump();
}

void INTERPRETER::parseSet() {
  std::cerr << "Set" << std::endl;

  if (!accept("write"))
    parseExpr();

  expect(',');

  if (!accept("read"))
    parseExpr();
}

void INTERPRETER::parseExpr() {
  std::cerr << "Expr" << std::endl;

  parseTerm();

  while (accept('+') || accept('-'))
    parseTerm();
}

void INTERPRETER::parseTerm() {
  std::cerr << "Term" << std::endl;

  parseFactor();

  while (accept('*') || accept('/') || accept('%'))
    parseFactor();
}

void INTERPRETER::parseFactor() {
  std::cerr << "Factor" << std::endl;

  if (accept("D[")) {
    parseExpr();

    expect(']');
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
    curIRIndex++;
    skipWhitespace();
    Arguments.push_back(IR[curIRIndex]);
    return;
  } else if (isdigit(IR[curIRIndex])) {
    while (curIRIndex < IR.length() &&
           isdigit(IR[curIRIndex])) {
      curIRIndex++;
    }
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

  expect(',');

  parseExpr();

  if (accept("!=") || accept("==") || accept(">=") || accept("<=") ||
      accept('>') || accept('<')) {
    parseExpr();
  } else {
    syntaxError();
  }
}

void INTERPRETER::syntaxError() {
  std::cerr << "Syntax Error!" << std::endl;
  exit(-1);
}

int main(int argc, char* argv[]) {
  std::cout << "**start**" << std::endl; 
  if (argc < 2) {
    std::cerr << "init: This program requires an input "
              << "filename as a parameter " << std::endl;
    exit(-2);
  }
  INTERPRETER i(argv[1]);
  i.runProgram();
  std::cout << "**end**" << std::endl;
}
