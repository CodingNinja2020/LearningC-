
// You are encouraged to challenge this code and notify me of issues :)
// Compare this code with Programming 1 code to realize differences

// From Model View perspective, this code mixes things. This makes real-life
// development harder due to coupling

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
#include "UsersManager.hpp"

using namespace std;

class AskMeSystem {
private:
  UsersManager users_manager;
  QuestionsManager questions_manager;

  void LoadDatabase(bool fill_user_questions = false) { // Internal
    users_manager.LoadDatabase();
    questions_manager.LoadDatabase();

    if (fill_user_questions) // first time, waiting for login
      ResetCurrentUserQuestions();
  }

  /*
   * Probably the most important design change
   * We needed to decouple the question manager from relying on User
   * implementation We break to 2 steps 1) Question manager return relevant
   * question 2) User manager helps reseting the user question. Even the manager
   * doesn't do this by itself
   *
   * This is more OO now
   */
  void ResetCurrentUserQuestions() {
    const User &user = users_manager.GetCurrentUser();

    const auto &to_questions = questions_manager.GetUserToQuestions(user);
    users_manager.ResetToQuestions(to_questions);

    const auto &from_questions = questions_manager.GetUserFromQuestions(user);
    users_manager.ResetFromQuestions(from_questions);
  }

public:
  void Run() { // only public one
    LoadDatabase(false);
    users_manager.AccessSystem();
    ResetCurrentUserQuestions();

    vector<string> menu;
    menu.push_back("Print Questions To Me");
    menu.push_back("Print Questions From Me");
    menu.push_back("Answer Question");
    menu.push_back("Delete Question");
    menu.push_back("Ask Question");
    menu.push_back("List System Users");
    menu.push_back("Feed");
    menu.push_back("Logout");

    while (true) {
      int choice = ShowReadMenu(menu);
      LoadDatabase(true);

      if (choice == 1)
        questions_manager.PrintUserToQuestions(users_manager.GetCurrentUser());
      else if (choice == 2)
        questions_manager.PrintUserFromQuestions(
            users_manager.GetCurrentUser());
      else if (choice == 3) {
        questions_manager.AnswerQuestion(users_manager.GetCurrentUser());
        questions_manager.UpdateDatabase();
      } else if (choice == 4) {
        questions_manager.DeleteQuestion(users_manager.GetCurrentUser());
        // Let's build again (just easier, but slow)
        ResetCurrentUserQuestions();
        questions_manager.UpdateDatabase();
      } else if (choice == 5) {
        pair<int, int> to_user_pair = users_manager.ReadUserId();
        if (to_user_pair.first != -1) {
          questions_manager.AskQuestion(users_manager.GetCurrentUser(),
                                        to_user_pair);
          questions_manager.UpdateDatabase();
        }
      } else if (choice == 6)
        users_manager.ListUsersNamesIds();
      else if (choice == 7)
        questions_manager.ListFeed();
      else
        break;
    }
    Run(); // Restart again
  }
};

int main() {
  AskMeSystem service;
  service.Run();

  return 0;
}

/*
 101,-1,11,13,0,Should I learn C++ first or Java,I think C++ is a better Start
 203,101,11,13,0,Why do you think so!,Just Google. There is an answer on Quora.
 205,101,45,13,0,What about python?,
 211,-1,13,11,1,It was nice to chat to you,For my pleasure Dr Mostafa
 212,-1,13,45,0,Please search archive before asking,
 300,101,11,13,1,Is it ok to learn Java for OOP?,Good choice
 301,-1,11,13,0,Free to meet?,
 302,101,11,13,1,Why so late in reply?,

 13,mostafa,111,mostafa_saad_ibrahim,mostafa@gmail.com,1
 11,noha,222,noha_salah,nono171@gmail.com,0
 45,ali,333,ali_wael,wael@gmail.com,0

 */
