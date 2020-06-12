#include <iostream>
#include <string>
#include <ctime>

#include "uint128.h"
#include "Schedule.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "Useage: sched.exe [autosched/modify].";
		return 0;
	}
	srand(time(NULL));
	stateOutputFile = "./state.json";

	string command;
	if (argc == 2) command = argv[1];
	if (command == "autosched") {
		Schedule sched;
		sched.LessonsFromJsonFile("./autosched_lessons.json");
		sched.RoomsFromJsonFile("./autosched_rooms.json");

		sched.DoSchedule();

		sched.OutputRes();
		cout << sched.DebugOutput();
	}
	else if (command == "modify") {
		Modify modi;
		modi.LessonFromJsonFile("./modify_lesson.json");
		modi.RoomFromJsonFile("./modify_room.json");
		modi.TargetTimeFromJsonFile("./modify_time.json");
		modi.TeacherFromJsonFile("./modify_teacher.json");

		modi.DoModify();

		modi.OutputRes();
	}
	else {
		cout << "Useage: sched.exe [autosched/modify].";
	}
	return 0;
}