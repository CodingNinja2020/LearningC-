#pragma once

#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

#include "Utils.hpp"
using namespace std;


class User {
private:
  int user_id; // internal system ID
  string user_name;
  string password;
  string name;
  string email;
  int allow_anonymous_questions; // 0 or 1

  vector<int> questions_id_from_me;
  // From question id to list of questions IDS on this question (thread
  // questions) - For this user
  map<int, vector<int>> questionid_questionidsThead_to_map;

public:
  User() : user_id(-1), allow_anonymous_questions(-1) {}

  User(const string &line) {
    vector<string> substrs = SplitString(line);
    assert(substrs.size() == 6);

    user_id = ToInt(substrs[0]);
    user_name = substrs[1];
    password = substrs[2];
    name = substrs[3];
    email = substrs[4];
    allow_anonymous_questions = ToInt(substrs[5]);
  }

  string ToString() const {
    ostringstream oss;
    oss << user_id << "," << user_name << "," << password << "," << name << ","
        << email << "," << allow_anonymous_questions;

    return oss.str();
  }

  void Print() const {
    cout << "User " << user_id << ", " << user_name << " " << password << ", "
         << name << ", " << email << "\n";
  }

  void ResetToQuestions(const vector<int> &to_questions) {
    questions_id_from_me.clear();

    for (const auto &question_id : to_questions)
      questions_id_from_me.push_back(question_id);
  }
  void ResetFromQuestions(const vector<pair<int, int>> &to_questions) {
    questionid_questionidsThead_to_map.clear();

    for (const auto &id_pair : to_questions)
      questionid_questionidsThead_to_map[id_pair.first].push_back(
          id_pair.second);
  }

  void ReadUser(const string &user_name, int id) {
    SetUserName(user_name);
    SetUserId(id);

    string str;

    cout << "Enter password: ";
    cin >> str;
    SetPassword(str);

    cout << "Enter name: ";
    cin >> str;
    SetName(str);

    cout << "Enter email: ";
    cin >> str;
    SetEmail(str);

    cout << "Allow anonymous questions? (0 or 1): ";
    int st;
    cin >> st;
    SetAllowAnonymousQuestions(st);
  }

  int IsAllowAnonymousQuestions() const { return allow_anonymous_questions; }

  void SetAllowAnonymousQuestions(int allowAnonymousQuestions) {
    allow_anonymous_questions = allowAnonymousQuestions;
  }

  const string &GetEmail() const { return email; }

  void SetEmail(const string &email) { this->email = email; }

  const string &GetName() const { return name; }

  void SetName(const string &name) { this->name = name; }

  const string &GetPassword() const { return password; }

  void SetPassword(const string &password) { this->password = password; }

  int GetUserId() const { return user_id; }

  void SetUserId(int userId) { user_id = userId; }

  const string &GetUserName() const { return user_name; }

  void SetUserName(const string &userName) { user_name = userName; }

  const map<int, vector<int>> &GetQuestionidQuestionidsTheadToMap() const {
    return questionid_questionidsThead_to_map;
    // Note no setter is provided / returned as const &
  }

  const vector<int> &GetQuestionsIdFromMe() const {
    return questions_id_from_me;
    // Note no setter is provided
  }
};
