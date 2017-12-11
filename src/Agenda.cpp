#include "AgendaSocket.hpp"
#include "AgendaUI.hpp"

int main() {
  std::cout << "input 1 for the AgendaUI and 2 for the AgendaSocket."
            << std::endl;
  int choice;
  std::cin >> choice;
  if (choice == 1) {
    AgendaUI aui;
    aui.OperationLoop();
  } else if (choice == 2) {
    AgendaSocket aserv;
    aserv.OperationLoop();
  }
  return 0;
}
