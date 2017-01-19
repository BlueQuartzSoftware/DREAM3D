/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                               *
 * Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe        *
 *                                                                                               *
 * This program is free software: you can redistribute it and/or modify it under the terms of    *
 * the GNU Lesser General Public License as published by the Free Software Foundation, either    *
 * version 2 of the License, or (at your option) any later version.                              *
 *                                                                                               *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 * See the GNU Lesser General Public License for more details.                                   *
 *                                                                                               *
 * You should have received a copy of the GNU Lesser General Public License                      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.                         *
 *                                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _PARSER_H_
#define _PARSER_H_

#include <cstdlib>
#include <cctype>
#include <cmath>

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stack>
#include <map>

//helper class to hold math tokens
template <typename T>
class Token {
  public:
    enum Type {Value, Variable, Operator, LParen, RParen, Separator, UrnaryFunction, BinaryFunction};//types of tokens
    enum Association {Left, Right};//types of associativity

    Token(std::string str, T x) : name(str), type(Value) {u.val = x;}//value
    Token(std::string str, T* x) : name(str), type(Variable) {u.var = x;}//variable
    Token(std::string str, T (*x)(const T, const T), size_t p, Association a) : name(str), type(Operator), precedence(p), associativity(a) {u.oper = x;}//operator
    Token(std::string str, Type t) : name(str), type(t) {}//LParen, RParen, and Separator
    Token(std::string str, T (*x)(const T)) : type(UrnaryFunction) {u.func1 = x;}//urnary function
    Token(std::string str, T (*x)(const T, const T)) : type(BinaryFunction) {u.func2 = x;}//binary function

    bool isFunction() {
      switch(type) {
        case UrnaryFunction://intentional fall through
        case BinaryFunction: return true;
        default: return false;
      }
    }

    bool isNumber() {
      switch(type) {
        case Value://intentional fall through
        case Variable: return true;
        default: return false;
      }
    }

    bool isLParen() {return (LParen == type);}
    bool isRParen() {return (RParen == type);}
    bool isSeparator() {return (Separator == type);}
    bool isOperator() {return (Operator == type);}

    union {
      T val;//value
      T* var;//variable
      T (*oper)(const T, const T);//binary operator
      T (*func1)(const T);//urnary function
      T (*func2)(const T, const T);//binary function
    } u;
    Type type;
    std::string name;
    size_t precedence;
    Association associativity;
};

template <typename T>
class Parser {
  public:
    Parser() : m_reserved("() \t\n\v\f\r"), m_separators(",") {defaultTokens();}

    /*
     * @brief addConstant() adds a constant token (e.g. pi)
     * @param name plaintext expression (e.g. "pi")
     * @param val constant value (e.g. 3.14159)
     */
    void addConstant(std::string name, T val) {addToken(name, Token<T>(name, val));}

    /*
     * @brief addVariable() adds a variable token (e.g. x)
     * @param name plaintext expression (e.g. "x")
     * @param var pointer to variable (e.g. &xVal)
     */ 
    void addVariable(std::string name, T* var) {addToken(name, Token<T>(name, var));}

    /*
     * @brief addFunction() adds an urnary function token (e.g. sin)
     * @param name plaintext expression (e.g. "sin")
     * @param func function pointer (e.g. std::sin)
     */ 
    void addFunction(std::string name, T (*func)(const T)) {addToken(name, Token<T>(name, func));}//urnary

    /*
     * @brief addFunction() adds a binay function token (e.g. atan2)
     * @param name plaintext expression (e.g. "atan2")
     * @param func function pointer (e.g. std::atan2)
     */ 
    void addFunction(std::string name, T (*func)(const T, const T)) {addToken(name, Token<T>(name, func));}//binary

    /*
     * @brief addOperator() adds an operator token (e.g. +)
     * @param oper function pointer (e.g. [](const T a, const T b){return a + b;})
     * @param name plaintext expression (e.g. "+")
     * @param prec operator precedence value
     * @param assoc operator associativity type
     */ 
    void addOperator(std::string name, T (*oper)(const T, const T), uint_fast8_t prec, typename Token<T>::Association assoc = Token<T>::Left) {addToken(name, Token<T>(name, oper, prec, assoc));}

    /*
     * @brief getExpression() get the currently set math expression
     * @return current expression
     */
    std::string getExpression() {return m_expression;}

    /*
     * @brief getRPN() get the currently set math expression in reverse polish notation
     * @return current expression
     */
    std::string getRPN();

    /*
     * @brief setExpression() set the expression to parse, throws runtime_error for bad expressions
     * @param expr math expression to use
     */
    void setExpression(std::string expr);
    
    /*
     * @brief evaluate() evaluates the current math expression, throws runtime_error for bad expressions
     * @return value
     */
    T evaluate();

  private:
    /*
     * @brief tokenize() tokenizes a plain text math expression, throws runtime_error for bad expressions
     * @param expr math expression to tokenize
     * @return vector of math Tokens in order
     */
    typename std::vector<Token<T> > tokenize(std::string expr);

    /*
     * @brief addToken() adds a token to the list of recognized expressions, throws runtime_error for bad expressions
     */
    void addToken(std::string name, Token<T> tok);

    /*
     * @brief defaultTokens() adds common / defualt tokens
     */
    void defaultTokens();

#if __cplusplus <= 199711L
    static inline T _min_T(const T a, const T b) {return (a < b) ? a : b;}
    static inline T _max_T(const T a, const T b) {return (a > b) ? a : b;}
    static inline T _add_T(const T a, const T b) {return a + b;}
    static inline T _sub_T(const T a, const T b) {return a - b;}
    static inline T _mul_T(const T a, const T b) {return a * b;}
    static inline T _div_T(const T a, const T b) {return a / b;}
#endif

    typename std::map<std::string, Token<T> > m_tokens;//map from recognized strings to token object
    std::string m_reserved;//reserved special characters (can't be start/end of token names)
    std::string m_separators;//characters used to seperate consecutive function arguments
    std::vector<Token <T> > m_rpn;//current expression in reverse polish notation
    std::string m_expression;//plain text expression
};

template <typename T>
void Parser<T>::addToken(std::string name, Token<T> tok) {
  //check front + back for reserved characters

#if __cplusplus > 199711L
  if(std::isdigit(name.front()) || std::string::npos != m_reserved.find(name.front()))
#else
  if(std::isdigit(name[0]) || std::string::npos != m_reserved.find(name[0]))
#endif
    {throw std::runtime_error(std::string("The token `" + name + "' starts with a reserved character").c_str());}
#if __cplusplus > 199711L
  if(std::string::npos != m_reserved.find(name.back()))
#else
  if(std::string::npos != m_reserved.find(*name.rbegin()))
#endif
    {throw std::runtime_error(std::string("The token `" + name + "' starts ends a reserved character").c_str());}

  //add if the name doesn't already exist
  if(!m_tokens.insert(typename std::pair<std::string, Token<T> >(name, tok)).second) {
    throw std::runtime_error(std::string("The token `" + name + "' already exists").c_str());}
}

template <typename T>
std::string Parser<T>::getRPN() {
  if(m_rpn.empty()) {return "";}
  std::stringstream ss;
  for(size_t i = 0; i < m_rpn.size(); i++) {ss << m_rpn[i].name << " ";}
  ss << "\b";
  return ss.str();
}

template <typename T>
void Parser<T>::defaultTokens(){
  //add special tokens
  m_tokens.insert(typename std::pair<std::string, Token<T> >("(", Token<T>("(", Token<T>::LParen)));
  m_tokens.insert(typename std::pair<std::string, Token<T> >(")", Token<T>(")", Token<T>::RParen)));
  for(size_t i = 0; i < m_separators.size(); i++) {
    std::string c = m_separators.substr(i, 1);
    m_tokens.insert(typename std::pair<std::string, Token<T> >(c, Token<T>(c, Token<T>::Separator)));
  }

  //add constants
  addConstant("pi", 3.14159265358979323846);

  //add urnary functions
  addFunction("sin", std::sin);
  addFunction("cos", std::cos);
  addFunction("tan", std::tan);
  addFunction("asin", std::asin);
  addFunction("acos", std::acos);
  addFunction("atan", std::atan);

  addFunction("sinh", std::sinh);
  addFunction("cosh", std::cosh);
  addFunction("tanh", std::tanh);
#if __cplusplus > 199711L
  addFunction("asinh", std::asinh);
  addFunction("acosh", std::acosh);
  addFunction("atanh", std::atanh);
#endif
  
  addFunction("exp", std::exp);
  addFunction("log", std::log);
  addFunction("log10", std::log10);
  addFunction("sqrt", std::sqrt);
  
  addFunction("ceil", std::ceil);
  addFunction("floor", std::floor);
  addFunction("fabs", std::fabs);
#if __cplusplus > 199711L
  addFunction("round", std::round);
  addFunction("trunc", std::trunc);

  addFunction("erf", std::erf);
  addFunction("erfc", std::erfc);
  addFunction("tgamma", std::tgamma);
  addFunction("lgamma", std::lgamma);

  //add binary functions
  addFunction("fmax", std::fmax);
  addFunction("fmin", std::fmin);
  addFunction("hypot", std::hypot);
#else
  addFunction("fmax", _max_T);
  addFunction("fmin", _min_T);
#endif
  addFunction("atan2", std::atan2);

  //add operators
#if __cplusplus > 199711L
  addOperator("+", [](const T a, const T b){return a + b;}, 2);
  addOperator("-", [](const T a, const T b){return a - b;}, 2);
  addOperator("*", [](const T a, const T b){return a * b;}, 3);
  addOperator("/", [](const T a, const T b){return a / b;}, 3);
#else
  addOperator("+", _add_T, 2);
  addOperator("-", _sub_T, 2);
  addOperator("*", _mul_T, 3);
  addOperator("/", _div_T, 3);
#endif
  addOperator("%", std::fmod, 4);
  addOperator("^", std::pow, 4, Token<T>::Right);
}

template <typename T>
typename std::vector<Token<T> > Parser<T>::tokenize(std::string expr) {
  size_t i = 0;
  typename std::vector<Token<T> > tokens;
  while(i < expr.size()) {
    typename std::map<std::string, Token<T> >::reverse_iterator it;//reverse iterate so e.g. log10 is before log
    for(it = m_tokens.rbegin(); it != m_tokens.rend(); ++it) {//search against known tokens
      if(0 == expr.compare(i, it->first.size(), it->first) ) {break;}
    }
    if(it != m_tokens.rend()) {//add token if found
      tokens.push_back(it->second);
      i += it->first.size();
      tokens.back().name = it->first;
    } else {//check other possibilities if not a known token
      if(std::isspace(expr[i])) {i++;}//skip white space
      else if(std::isdigit(expr[i])) {//parse numbers
        size_t j = expr.find_first_not_of("0.123456789eE", i);
        std::string num = expr.substr(i, j - i);
        tokens.push_back( Token<T>(num, (T) atof(num.c_str()) ) );
        tokens.back().name = expr.substr(i, j - i);
        i = j;
      } else {throw std::runtime_error("unknown token \"" + expr.substr(i, 5) + "...\"");}
    }
  }
  return tokens;
}

template <typename T>
void Parser<T>::setExpression(std::string expr) {
  //tokenize expression and sanity check adjacency
  typename std::vector<Token<T> > inputTokens = tokenize(expr);
  for(size_t i = 1; i < inputTokens.size(); i++) {
    if(inputTokens[i - 1].isFunction() && !inputTokens[i].isLParen() ) {
      throw std::runtime_error(std::string("functions must be followed by '(' (did you mean \"" + inputTokens[i - 1].name + "(" + inputTokens[i].name + "\"?)").c_str());
    }

    if(inputTokens[i - 1].isNumber() && inputTokens[i].isNumber()   ||
       inputTokens[i - 1].isNumber() && inputTokens[i].isLParen()   ||
       inputTokens[i - 1].isNumber() && inputTokens[i].isFunction() || 
       inputTokens[i - 1].isRParen() && inputTokens[i].isNumber()   ||
       inputTokens[i - 1].isRParen() && inputTokens[i].isLParen()   ||
       inputTokens[i - 1].isRParen() && inputTokens[i].isFunction() ) {
      throw std::runtime_error(std::string("consecutive values disallowed (did you mean \"" + inputTokens[i - 1].name + "*" + inputTokens[i].name + "\"?)").c_str());
    }
  
    if(inputTokens[i - 1].isLParen()    && inputTokens[i].isRParen()    ||
       inputTokens[i - 1].isLParen()    && inputTokens[i].isSeparator() ||
       inputTokens[i - 1].isLParen()    && inputTokens[i].isOperator()  ||
       inputTokens[i - 1].isSeparator() && inputTokens[i].isRParen()    ||
       inputTokens[i - 1].isSeparator() && inputTokens[i].isSeparator() ||
       inputTokens[i - 1].isSeparator() && inputTokens[i].isOperator()  ||
       inputTokens[i - 1].isOperator()  && inputTokens[i].isRParen()    ||
       inputTokens[i - 1].isOperator()  && inputTokens[i].isSeparator() ||
       inputTokens[i - 1].isOperator()  && inputTokens[i].isOperator() ) {
      throw std::runtime_error(std::string("expected value between \"" + inputTokens[i - 1].name + "\" and \"" + inputTokens[i].name + "\"").c_str());
    }
  }

  //loop over tokens converting expression to reverse polish notation via shunting yard algorithm
  typename std::vector<Token<T> > rpn;
  typename std::stack<Token<T> > stack;
  for(size_t i = 0; i < inputTokens.size(); i++) {
    switch(inputTokens[i].type) {
      //values are added directly to rpn expression
      case Token<T>::Value://intentional fall through
      case Token<T>::Variable:
        rpn.push_back(inputTokens[i]);
        break;

      //functions and left parenthesis are added to operator stack
      case Token<T>::UrnaryFunction://intentional fall through
      case Token<T>::BinaryFunction://intentional fall through
      case Token<T>::LParen:
        stack.push(inputTokens[i]);
        break;

      //argument seperators trigger pulling from stack to rpn until left parenthesis (function start) is found
      case Token<T>::Separator:
        while(true) {
          if(stack.empty()) {throw std::runtime_error("misplaced argument separator (no preceding 'function(' found)");}
          if(stack.top().isLParen()) {break;}
          rpn.push_back(stack.top());
          stack.pop();
        }
      break;

      //operators trigger pulling from stack to rpn until the top is lower precedence
      case Token<T>::Operator:
        while(!stack.empty()) {
          if(!stack.top().isOperator()) {break;}
          if( (inputTokens[i].associativity == Token<T>::Left && inputTokens[i].precedence <= stack.top().precedence) || 
              (inputTokens[i].associativity == Token<T>::Right && inputTokens[i].precedence < stack.top().precedence) ) {
            rpn.push_back(stack.top());
            stack.pop();
          } else {break;}
        }
        stack.push(inputTokens[i]);
      break;

      //pull from stack to rpn until closing parenthesis found and add preceding function if necessary
      case Token<T>::RParen:
        while(true) {
          if(stack.empty()) {throw std::runtime_error("unmatched ')' (no preceding '(' found)");}
          if(stack.top().isLParen()) {break;}
          rpn.push_back(stack.top());
          stack.pop();
        }
        stack.pop();//remove '(' from stack
        if(!stack.empty()) {
          if(stack.top().isFunction()) {//grab function before '(' if needed
            rpn.push_back(stack.top());
            stack.pop();
          }
        }
        break;
    }
  }

  //pull remaining operators from stack when input queue is exhuasted
  while(!stack.empty()) {
    if(stack.top().isLParen()) {throw std::runtime_error("unmatched '(' (no following ')' found)");}
    rpn.push_back(stack.top());
    stack.pop();
  }

  //double check rpn validity by evaluating
  std::swap(m_rpn, rpn);
  try {
    evaluate();
  } catch (...) {
    std::swap(m_rpn, rpn);//retain old expression if the new one is bad
    throw;
  }
  m_expression = expr;
}

template <typename T>
T Parser<T>::evaluate() {
  if(m_rpn.empty()) {throw std::runtime_error("no expression set");}

  typename std::stack<T> stack;
  for(size_t i = 0; i < m_rpn.size(); i++) {
    switch(m_rpn[i].type) {
      //values are pushed to the argument stack
      case Token<T>::Value:
        stack.push(m_rpn[i].u.val);
        break;

      case Token<T>::Variable: 
        stack.push(*m_rpn[i].u.var);
        break;

      //functions / operators are executed by pulling arguments from the stack then pushing the result back
      case Token<T>::Operator: {
        T arg[2];
        if(stack.size() < 2) {throw std::runtime_error(std::string("not enough arguments for operator `" + m_rpn[i].name +"'").c_str());}
        for(size_t j = 0; j < 2; j++) {//get arguments from stack
          arg[j] = stack.top();
          stack.pop();
        }
        stack.push(m_rpn[i].u.oper(arg[1], arg[0]));//push result to stack
      } break;

      case Token<T>::UrnaryFunction: {
        if(stack.empty()) {throw std::runtime_error(std::string("not enough arguments for function `" + m_rpn[i].name +"'").c_str());}
        T v = stack.top();
        stack.pop();
        stack.push(m_rpn[i].u.func1(v));
      } break;

      case Token<T>::BinaryFunction: {
        T arg[2];
        if(stack.size() < 2) {throw std::runtime_error(std::string("not enough arguments for operator `" + m_rpn[i].name +"'").c_str());}
        for(size_t j = 0; j < 2; j++) {//get arguments from stack
          arg[j] = stack.top();
          stack.pop();
        }
        stack.push(m_rpn[i].u.func2(arg[1], arg[0]));//push result to stack
      } break;
    }
  }

  //valid expressions have only the answer remaining in the stack
  if(1 != stack.size()) {throw std::runtime_error("too many arguments provided");}
  return stack.top();
}

#endif