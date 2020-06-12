#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<memory>
#include<cstdlib>
#include<algorithm>

#include "json.hpp"
#include"uint128.h"

// If the json provided has name mismatch, modify the following defines.
#if 1
// json entrances for Lesson
#define LESSON_ID "ID"
#define LESSON_TEACHERID "teacher_ID"
#define LESSON_CAPACITY "cap"
#define LESSON_TIME "class_time"
#define LESSON_DURATION "length"
#define LESSON_EXAMTIME "exam_time"
#define LESSON_POSITION "position"

// json entrances for Room
#define ROOM_ID "ID"
#define ROOM_CAPACITY "cap"
#define ROOM_TIME "room_time"

// json entrances for Teacher
#define TEACHER_ID "ID"
#define TEACHER_TIME "teacher_time"

// json entrances for State output
#define STATE_SUCCESS "success"
#define STATE_INFO "info"
#define STATE_ROOMS "rooms"
#define STATE_LESSONS "lessons"
#define STATE_TEACHERS "teachers"

#define STATE_SINGLEROOM "room"
#define STATE_SINGLELESSON "lesson"
#define STATE_SINGLETEACHER "teacher"

// json entrances for modify
#define MODIFY_TIME "target_time"
#endif

#define JSONLENONCE 400
#define SPACECHARS " \t\n"

using namespace std;
extern string stateOutputFile;

void defaultErrorHandler(string errorInfo);

/* LessonInfo: A class for reading and writing lesson data in json.
	And provide data for scheduling.
*/
class LessonInfo {
public:
	LessonInfo() = default;
	void FromJsonFile(string path);
	void FromJsonStr(string json);
	string DebugOutput() const noexcept;
	string ModifyOutput() const noexcept;

	void SetClassTime(uint128 classtime) { classTime = classtime; }
	void SetPosition(int roomid) { position = roomid; }
	
	int GetClassID() const { return classID; }
	int GetTeacherID() const { return teacherID; }
	uint128 GetClassTime() const { return classTime; }
	int GetLength() const { return length; }
	int GetPosition() const { return position; }
	int GetCapacity() const { return capacity; }

	int GetLessonPeriods() const noexcept;
	int GetNextPeriodLen() const noexcept;
	void CutPeriodLen() noexcept;

	void ResetForSched() noexcept {
		classTime = 0;
		position = -1;
	}

private:
	int classID, teacherID;
	uint128 classTime;
	int length;
	int examTime;
	int position;
	int capacity;
	int restLen;

	void (*errorHandler)(string) = defaultErrorHandler;
};
typedef shared_ptr<LessonInfo> LessonPtr;

/* RoomInfo: A class for reading and writing room data in json.
	And provide data for scheduling.
*/
class RoomInfo {
public:
	RoomInfo() = default;
	void FromJsonFile(string pathStr);
	void FromJsonStr(string jsonStr);
	string DebugOutput() const noexcept;
	string ModifyOutput() const noexcept;

	void AddUse(uint128 addition) { useTime = useTime | addition; }

	int GetRoomID() const { return roomID; }
	int GetCapacity() const { return capacity; }
	uint128 GetUseTime() const { return useTime; }

	void SetUsetTime(uint128 newtime) { useTime = newtime; }

	void ResetForSched() {
		useTime = 0;
	}

private:
	int roomID;
	int capacity;
	uint128 useTime;

	void (*errorHandler)(string) = defaultErrorHandler;
};
typedef shared_ptr<RoomInfo> RoomPtr;
bool SortRoomPtr(const RoomPtr& p1, const RoomPtr& p2);

class Schedule;
/* TeacherInfo: A class that stores the data of teachers.
	This class provides the data of priority to the schedule algorithm.
*/
class TeacherInfo {
public:
	TeacherInfo() = default;
	TeacherInfo(int ID) :ID(ID), lessonCnt(0), busyTime() {};

	void FromJsonFile(string pathStr);
	void FromJsonStr(string jsonStr);

	void AddLesson(LessonPtr lesson);

	int GetID() const noexcept { return ID; }
	int GetLessonCount() const noexcept { return lessonCnt; }
	uint128 GetBusyTime() const noexcept { return busyTime; }

	void SetBusyTime(uint128 newTime) { busyTime = newTime; }

	string OutputSchedule() const noexcept;

	string ModifyOutput();

	void ResetForSched() {
		busyTime = 0;
	}

	friend class Schedule;

private:
	int ID;
	int lessonCnt;
	uint128 busyTime;
	vector<LessonPtr> lessons;

	void (*errorHandler)(string) = defaultErrorHandler;

};
typedef shared_ptr<TeacherInfo> TeacherPtr;
bool SortTeacherPtr(const TeacherPtr& p1, const TeacherPtr& p2);

/* RoomList: A class for reading and writing the data of all rooms.
	
*/
class RoomList {
public:
	RoomList() = default;

	void FromJsonFile(string pathStr);
	void FromJsonStr(string jsonStr);
	string GetRoomModify() const;
	
	void Sort() { sort(rooms.begin(), rooms.end(), SortRoomPtr); }
	
	LessonPtr AllocRoomFor(uint128 busytime, int totalLen, int cap);
	uint128 FitInto(uint128 possibleTime, int totalLen, bool relaxed) const noexcept;

	void ResetForSched() { memset(useCount, 0, sizeof(useCount)); for (int i = 0; i < rooms.size(); ++i) rooms[i]->ResetForSched(); }
private:
	//7 days/week, 5 periods/day.
	int useCount[8] = { 0 };
	vector<RoomPtr> rooms;

	uint128 GetTimeMask(int level, uint128 busytime) const noexcept;

	void (*errorHandler)(string) = defaultErrorHandler;
};

/* Schedule: A class for reading data and schedule the lessons.

*/
class Schedule {
public:
	Schedule() = default;

	void RoomsFromJsonFile(string pathStr);
	void RoomsFromJsonStr(string jsonStr);

	void LessonsFromJsonFile(string pathStr);
	void LessonsFromJsonStr(string jsonStr);

	string GetLessonModify() const;
	string GetTeacherModify() const;

	void DoSchedule();
	bool IsScheduleSuccess() const { return isSuccess; }

	void OutputRes();

	void ResetForSched() { rooms.ResetForSched(); for (int i = 0; i < teachers.size(); ++i)teachers[i]->ResetForSched(); isSuccess = false; }

	string DebugOutput() const noexcept;
private:
	RoomList rooms;
	vector<TeacherPtr> teachers;

	bool isSuccess = false;

	void (*errorHandler)(string) = defaultErrorHandler;
};

class Modify {
public:
	Modify() = default;

	void RoomFromJsonFile(string pathStr) { room.FromJsonFile(pathStr); }
	void RoomFromJsonStr(string jsonStr) { room.FromJsonStr(jsonStr); }
	void LessonFromJsonFile(string pathStr) { lesson.FromJsonFile(pathStr); }
	void LessonFromJsonStr(string jsonStr) { lesson.FromJsonStr(jsonStr); }
	void TeacherFromJsonFile(string pathStr) { teacher.FromJsonFile(pathStr); }
	void TeacherFromJsonStr(string jsonStr) { teacher.FromJsonStr(jsonStr); }

	void TargetTimeFromJsonFile(string pathStr);
	void TargetTimeFromJsonStr(string jsonStr);

	void DoModify();

	void OutputRes();

private:
	uint128 targetTime;
	RoomInfo room;
	LessonInfo lesson;
	TeacherInfo teacher;

	bool isSuccess = false;

	void (*errorHandler)(string) = defaultErrorHandler;
};