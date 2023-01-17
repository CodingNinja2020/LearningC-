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
using namespace std;


class QuestionsManager {
private:
  // From question id to list of questions IDS on this question (thread
  // questions) - All users
  map<int, vector<int>> questionid_questionidsThead_to_map;

  // Map the question id to question object. Let's keep one place ONLY with the
  // object When you study pointers, easier handling
  map<int, Question> questionid_questionobject_map;

  int last_id;

public:
  QuestionsManager() { last_id = 0; }

  void LoadDatabase() {
    last_id = 0;
    questionid_questionidsThead_to_map.clear();
    questionid_questionobject_map.clear();

    vector<string> lines = ReadFileLines("questions.txt");
    for (const auto &line : lines) {
      Question question(line);
      last_id = max(last_id, question.GetQuestionId());

      questionid_questionobject_map[question.GetQuestionId()] = question;

      if (question.GetParentQuestionId() == -1)
        questionid_questionidsThead_to_map[question.GetQuestionId()].push_back(
            question.GetQuestionId());
      else
        questionid_questionidsThead_to_map[question.GetParentQuestionId()]
            .push_back(question.GetQuestionId());
    }
  }

  vector<int> GetUserToQuestions(const User &user) const {
    vector<int> question_ids;

    for (const auto &pair :
         questionid_questionidsThead_to_map) { // pair<int, vector<Question>>
      for (const auto &question_id : pair.second) { //  vector<Question>

        // Get the question from the map. & means same in memory, DON'T COPY
        const Question &question =
            questionid_questionobject_map.find(question_id)->second;

        if (question.GetFromUserId() == user.GetUserId())
          question_ids.push_back(question.GetQuestionId());
      }
    }
    return question_ids;
  }

  vector<pair<int, int>> GetUserFromQuestions(const User &user) const {
    vector<pair<int, int>> question_ids;

    for (const auto &pair :
         questionid_questionidsThead_to_map) { // pair<int, vector<Question>>
      for (const auto &question_id : pair.second) { //  vector<Question>
        // Get the question from the map. & means same in memory, DON'T COPY
        const Question &question =
            questionid_questionobject_map.find(question_id)->second;

        if (question.GetToUserId() == user.GetUserId()) {
          if (question.GetParentQuestionId() == -1)
            question_ids.push_back(
                make_pair(question.GetQuestionId(), question.GetQuestionId()));
          else
            question_ids.push_back(make_pair(question.GetParentQuestionId(),
                                             question.GetQuestionId()));
        }
      }
    }
    return question_ids;
  }

  void PrintUserToQuestions(const User &user) const {
    cout << "\n";

    if (user.GetQuestionidQuestionidsTheadToMap().size() == 0)
      cout << "No Questions";

    for (const auto &pair :
         user.GetQuestionidQuestionidsTheadToMap()) { // pair<int,
                                                      // vector<Question>>
      for (const auto &question_id : pair.second) {   //  vector<Question>

        // Get the question from the map. & means same in memory, DON'T COPY
        // Accessing questionid_questionobject_map[] change the map by adding if
        // not exist. You can't use in const function Here we just find, which
        // return iterator (second is like a pointer to object)
        const Question &question =
            questionid_questionobject_map.find(question_id)->second;
        question.PrintToQuestion();
      }
    }
    cout << "\n";
  }

  void PrintUserFromQuestions(const User &user) const {
    cout << "\n";
    if (user.GetQuestionsIdFromMe().size() == 0)
      cout << "No Questions";

    for (const auto &question_id :
         user.GetQuestionsIdFromMe()) { //  vector<Question>
      const Question &question =
          questionid_questionobject_map.find(question_id)->second;
      question.PrintFromQuestion();
    }
    cout << "\n";
  }

  // Used in Answering a question for YOU.
  // It can be any of your questions (thread or not)
  int ReadQuestionIdAny(const User &user) const {
    int question_id;
    cout << "Enter Question id or -1 to cancel: ";
    cin >> question_id;

    if (question_id == -1)
      return -1;

    if (!questionid_questionobject_map.count(question_id)) {
      cout << "\nERROR: No question with such ID. Try again\n\n";
      return ReadQuestionIdAny(user);
    }
    const Question &question =
        questionid_questionobject_map.find(question_id)->second;

    if (question.GetToUserId() != user.GetUserId()) {
      cout << "\nERROR: Invalid question ID. Try again\n\n";
      return ReadQuestionIdAny(user);
    }
    return question_id;
  }

  // Used to ask a question on a specific thread for whatever user
  int ReadQuestionIdThread(const User &user) const {
    int question_id;
    cout << "For thread question: Enter Question id or -1 for new question: ";
    cin >> question_id;

    if (question_id == -1)
      return -1;

    if (!questionid_questionidsThead_to_map.count(question_id)) {
      cout << "No thread question with such ID. Try again\n";
      return ReadQuestionIdThread(user);
    }
    return question_id;
  }

  void AnswerQuestion(const User &user) {
    int question_id = ReadQuestionIdAny(user);

    if (question_id == -1)
      return;

    Question &question =
        questionid_questionobject_map.find(question_id)->second;

    question.PrintToQuestion();

    if (question.GetAnswerText() != "")
      cout << "\nWarning: Already answered. Answer will be updated\n";

    cout << "Enter answer: "; // if user entered comma, system fails :)

    string line;
    getline(cin, line);
    getline(cin, line);
    question.SetAnswerText(line);
  }

  void DeleteQuestion(const User &user) {
    int question_id = ReadQuestionIdAny(user);

    if (question_id == -1)
      return;

    vector<int> ids_to_remove; // to remove from questionid_questionobject_map

    // Let's see if thread or not. If thread, remove all of it
    if (questionid_questionidsThead_to_map.count(question_id)) { // thread
      ids_to_remove = questionid_questionidsThead_to_map[question_id];
      questionid_questionidsThead_to_map.erase(question_id);
    } else {
      ids_to_remove.push_back(question_id);

      // let's find in which thread to remove. Consistency is important when
      // have multi-view
      for (auto &pair : questionid_questionidsThead_to_map) {
        vector<int> &vec = pair.second;
        for (int pos = 0; pos < (int)vec.size(); ++pos) {
          if (question_id == vec[pos]) {
            vec.erase(vec.begin() + pos);
            break;
          }
        }
      }
    }
    for (const auto &id : ids_to_remove)
      questionid_questionobject_map.erase(id);
  }

  void AskQuestion(const User &user, const pair<int, int> &to_user_pair) {
    Question question;

    if (!to_user_pair.second) {
      cout << "Note: Anonymous questions are not allowed for this user\n";
      question.SetIsAnonymousQuestions(0);
    } else {
      cout << "Is anonymous questions?: (0 or 1): ";
      int st;
      cin >> st;
      question.SetIsAnonymousQuestions(st);
    }

    question.SetParentQuestionId(ReadQuestionIdThread(user));

    cout << "Enter question text: "; // if user entered comma, system fails :)
    string line;
    getline(cin, line);
    getline(cin, line);
    question.SetQuestionText(line);

    question.SetFromUserId(user.GetUserId());
    question.SetToUserId(to_user_pair.first);

    // What happens in 2 parallel sessions who asked question?
    // They are given same id. This is wrong handling :)
    question.SetQuestionId(++last_id);

    questionid_questionobject_map[question.GetQuestionId()] = question;

    if (question.GetParentQuestionId() == -1)
      questionid_questionidsThead_to_map[question.GetQuestionId()].push_back(
          question.GetQuestionId());
    else
      questionid_questionidsThead_to_map[question.GetParentQuestionId()]
          .push_back(question.GetQuestionId());
  }

  void ListFeed() const {
    for (const auto &pair : questionid_questionobject_map) {
      const Question &question = pair.second;

      if (question.GetAnswerText() == "")
        continue;
      question.PrintFeedQuestion();
    }
  }

  void UpdateDatabase() {
    vector<string> lines;

    for (const auto &pair : questionid_questionobject_map)
      lines.push_back(pair.second.ToString());

    WriteFileLines("questions.txt", lines, false);
  }
};
