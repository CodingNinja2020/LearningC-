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
#include "Question.hpp"
#include "User.hpp"
#include "QuestionsManager.hpp"

using namespace std;


class UsersManager {
private:
  map<string, User> userame_userobject_map;
  User current_user;
  int last_id;

public:
  UsersManager() { last_id = 0; }

  void LoadDatabase() {
    last_id = 0;
    userame_userobject_map.clear();

    vector<string> lines = ReadFileLines("users.txt");
    for (const auto &line : lines) {
      User user(line);
      userame_userobject_map[user.GetUserName()] = user;
      last_id = max(last_id, user.GetUserId());
    }
  }

  void AccessSystem() {
    int choice = ShowReadMenu({"Login", "Sign Up"});
    if (choice == 1)
      DoLogin();
    else
      DoSignUp();
  }

  void DoLogin() {
    LoadDatabase(); // in case user added from other parallel run

    while (true) {
      string name, pass;
      cout << "Enter user name & password: ";
      cin >> name >> pass;
      current_user.SetUserName(name);
      current_user.SetPassword(pass);

      if (!userame_userobject_map.count(current_user.GetUserName())) {
        cout << "\nInvalid user name or password. Try again\n\n";
        continue;
      }
      const User &user_exist =
          userame_userobject_map[current_user.GetUserName()];

      if (current_user.GetPassword() != user_exist.GetPassword()) {
        cout << "\nInvalid user name or password. Try again\n\n";
        continue;
      }
      current_user = user_exist;
      break;
    }
  }

  void DoSignUp() {
    string user_name;
    while (true) {
      cout << "Enter user name. (No spaces): ";
      cin >> user_name;

      if (userame_userobject_map.count(user_name))
        cout << "Already used. Try again\n";
      else
        break;
    }
    // Move logic to user class, which may keep extending data members in future

    current_user.ReadUser(user_name, ++last_id);
    userame_userobject_map[current_user.GetUserName()] = current_user;

    UpdateDatabase(current_user);
  }

  void ResetToQuestions(const vector<int> &to_questions) {
    current_user.ResetToQuestions(to_questions);
  }

  void ResetFromQuestions(const vector<pair<int, int>> &to_questions) {
    current_user.ResetFromQuestions(to_questions);
  }

  void ListUsersNamesIds() const {
    for (const auto &pair : userame_userobject_map)
      cout << "ID: " << pair.second.GetUserId()
           << "\t\tName: " << pair.second.GetName() << "\n";
  }

  pair<int, int> ReadUserId() const {
    int user_id;
    cout << "Enter User id or -1 to cancel: ";
    cin >> user_id;

    if (user_id == -1)
      return make_pair(-1, -1);

    for (const auto &pair : userame_userobject_map) {
      if (pair.second.GetUserId() == user_id)
        return make_pair(user_id, pair.second.IsAllowAnonymousQuestions());
    }

    cout << "Invalid User ID. Try again\n";
    return ReadUserId();
  }

  void UpdateDatabase(const User &user) {
    string line = user.ToString();
    vector<string> lines(1, line);
    WriteFileLines("users.txt", lines);
  }

  const User &GetCurrentUser() const { return current_user; }
};
