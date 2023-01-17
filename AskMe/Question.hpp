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

class Question {
private:
  int question_id;
  // To support thread. Each question look to a parent question
  // -1 No parent (first question in the thread)
  int parent_question_id;
  int from_user_id;
  int to_user_id;
  int is_anonymous_questions; // 0 or 1
  string question_text;
  string answer_text; // empty = not answered

public:
  /*
   * How many conclassor arguments is too many?
   * https://stackoverflow.com/questions/40264/how-many-conclassor-arguments-is-too-many
   */
  Question()
      : question_id(-1), parent_question_id(-1), from_user_id(-1),
        to_user_id(-1), is_anonymous_questions(1) {}

  Question(const string &line) {
    vector<string> substrs = SplitString(line);
    assert(substrs.size() == 7);

    question_id = ToInt(substrs[0]);
    parent_question_id = ToInt(substrs[1]);
    from_user_id = ToInt(substrs[2]);
    to_user_id = ToInt(substrs[3]);
    is_anonymous_questions = ToInt(substrs[4]);
    question_text = substrs[5];
    answer_text = substrs[6];
  }

  // It is more proper (or safer) to use the getters/setters in the class
  // itself. For simplicity, I did not apply this in the code.

  string ToString() const {
    ostringstream oss;
    oss << question_id << "," << parent_question_id << "," << from_user_id
        << "," << to_user_id << "," << is_anonymous_questions << ","
        << question_text << "," << answer_text;

    return oss.str();
  }

  void PrintToQuestion() const {
    string prefix = "";

    if (parent_question_id != -1)
      prefix = "\tThread: ";

    cout << prefix << "Question Id (" << question_id << ")";
    if (!is_anonymous_questions)
      cout << " from user id(" << from_user_id << ")";
    cout << "\t Question: " << question_text << "\n";

    if (answer_text != "")
      cout << prefix << "\tAnswer: " << answer_text << "\n";
    cout << "\n";
  }

  void PrintFromQuestion() const {
    cout << "Question Id (" << question_id << ")";
    if (!is_anonymous_questions)
      cout << " !AQ";

    cout << " to user id(" << to_user_id << ")";
    cout << "\t Question: " << question_text;

    if (answer_text != "")
      cout << "\tAnswer: " << answer_text << "\n";
    else
      cout << "\tNOT Answered YET\n";
  }

  void PrintFeedQuestion() const {
    if (parent_question_id != -1)
      cout << "Thread Parent Question ID (" << parent_question_id << ") ";

    cout << "Question Id (" << question_id << ")";
    if (!is_anonymous_questions)
      cout << " from user id(" << from_user_id << ")";

    cout << " To user id(" << to_user_id << ")";

    cout << "\t Question: " << question_text << "\n";
    if (answer_text != "")
      cout << "\tAnswer: " << answer_text << "\n";
  }
  /////////////////
  // Setters & Getters

  const string &GetAnswerText() const { return answer_text; }

  void SetAnswerText(const string &answerText) { answer_text = answerText; }

  int GetFromUserId() const { return from_user_id; }

  void SetFromUserId(int fromUserId) { from_user_id = fromUserId; }

  int GetIsAnonymousQuestions() const { return is_anonymous_questions; }

  void SetIsAnonymousQuestions(int isAnonymousQuestions) {
    is_anonymous_questions = isAnonymousQuestions;
  }

  int GetParentQuestionId() const { return parent_question_id; }

  void SetParentQuestionId(int parentQuestionId) {
    parent_question_id = parentQuestionId;
  }

  int GetQuestionId() const { return question_id; }

  void SetQuestionId(int questionId) { question_id = questionId; }

  const string &GetQuestionText() const { return question_text; }

  void SetQuestionText(const string &questionText) {
    question_text = questionText;
  }

  int GetToUserId() const { return to_user_id; }

  void SetToUserId(int toUserId) { to_user_id = toUserId; }
};
