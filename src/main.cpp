#include<iostream>
#include<fstream>
#include<sstream>
#include<optional>
#include<vector>

enum class TokenType{

  exit,
  int_lit,
  semi

};

struct Token{

  TokenType type;
  std::optional <std::string> value;

};

std::vector<Token> tokenize(const std::string& str){

  std::string buff;
  std::vector<Token> tokens {};

  for(int i = 0; i < str.length(); i++){
    char c = str.at(i);

    if(std::isalpha(c)){
      buff.push_back(c);
      i++;
    
      while(std::isalnum(str.at(i))){
        buff.push_back(str.at(i));
        i++;
      }
      i--;
      
      if(buff == "exit"){ 
        tokens.push_back({.type = TokenType::exit});
        buff.clear();
      }
      else{  
        std::cerr << "Invalid usage" << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    else if(std::isdigit(c)){
      buff.push_back(c);
      i++;
      while(std::isdigit(str.at(i))){
        buff.push_back(str.at(i));
        i++;
      }
      i--;
      tokens.push_back({.type = TokenType::int_lit, .value = buff});
      buff.clear();
    }

    else if (c == ';'){
      tokens.push_back({.type = TokenType::semi});
    }

    else if(std::isspace(c)){
      continue;
    }

    else{  
      std::cerr << "Invalid usage" << std::endl;
      exit(EXIT_FAILURE);
    }

  }

  return tokens;

}

std::string tokens_to_asm(const std::vector<Token>& tokens){

  std::stringstream output;
  output << "global _start\n_start:\n";

  for(int i = 0; i < tokens.size(); i++){
    const Token& token = tokens.at(i);
    if(token.type == TokenType::exit){
      if(i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit){
        if(i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi){
          output << "    mov rax, 60,\n";
          output << "    mov rdi, " << tokens.at(i + 1).value.value() << "," << std::endl;
          output << "    syscall\n";
        }
      }
    }
  }
  return output.str();
}

int main(int argc, char* argv[]){

  if(argc != 2){
    std::cerr << "incorrect usage" << std::endl;
    std::cerr << "correct usage: ava <filename.av>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string contents;
  {
    std::fstream input(argv[1], std::ios::in);
    if(!input){
      std::cerr << "Failed to open file: " << argv[1] << std::endl;
      return EXIT_FAILURE;
    };
  
    
    std::stringstream contents_stream;
    contents_stream << input.rdbuf();
    contents = contents_stream.str();
  }

  std::vector<Token> tokens = tokenize(contents);
  
  {
  std::fstream file("out.asm", std::ios::out);
    file << tokens_to_asm(tokens);
  }

  system("nasm -felf64 out.asm");
  system("ld -o out out.o");
  system("./out");

  return EXIT_SUCCESS;
}
