#include "Schedule.h"
#include <fstream>
#include <cstdlib>
#include <exception>
using namespace std;

string stateOutputFile;

/* Initialize LessonInfo from a json file.
 * @param:
 * pathStr: The path of the json file.
 * @retval: none
 * Note: The file cannot be longer than 500 chars by default.
 * In this case, you should change the value of MAXJSONLEN.
*/
void LessonInfo::FromJsonFile(string pathStr)
try {
	ifstream fin(pathStr, fstream::in);
	if (!fin.is_open()) throw string("Failed opening input file. Is the filename or path wrong?");
	char jsonFile[JSONLENONCE + 1] = { 0 };
	string jsonStr;
	fin.read(jsonFile, JSONLENONCE);
	while (!fin.eof()) {
		jsonStr += jsonFile;
		fin.read(jsonFile, JSONLENONCE);
	}
	//	if (!fin.eof()) throw string("Expected EOF not met. Is json file longer than MAXJSONLEN?");
	jsonStr += jsonFile;
	fin.close();

	FromJsonStr(jsonStr);
}
catch (exception ex) {
	errorHandler(string("LessonInfo::fromJsonFile: ") + ex.what());
}
catch (string ex) {
	errorHandler(string("LessonInfo::fromJsonFile: ") + ex);
}
catch (...) {
	errorHandler(string("LessonInfo::fromJsonFile: Something is wrong."));
}

/* Initialize LessonInfo from a json string.
 * @param:
 * jsonStr: The json string.
 * @retval: none
 * Note: The file cannot be longer than 500 chars by default.
 * In this case, you should change the value of MAXJSONLEN.
*/
void LessonInfo::FromJsonStr(string jsonStr)
try {
	nlohmann::json stringIn;

	// Avoid overflow for 128bit.
	int pos1, pos2;
	pos1 = jsonStr.find(string(LESSON_TIME));
	if (pos1 == string::npos) throw "Expected " + string(LESSON_TIME) + " not found. Is the json string complete?";
	pos1 += strlen(LESSON_TIME) + 1;

	while (pos1 < jsonStr.length() && (jsonStr[pos1] < '0' || jsonStr[pos1] > '9')) pos1++;
	if (pos1 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");
	pos2 = pos1;
	while (pos2 < jsonStr.length() && (jsonStr[pos2] >= '0' && jsonStr[pos2] <= '9')) pos2++;
	if (pos2 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");
	classTime = jsonStr.substr(pos1,pos2-pos1);
	for (int i = pos1; i < pos2 - 1; ++i) jsonStr[i] = ' ';

	stringIn = nlohmann::json::parse(jsonStr);
	classID = stringIn[LESSON_ID];
	teacherID = stringIn[LESSON_TEACHERID];
	capacity = stringIn[LESSON_CAPACITY];
	restLen = length = stringIn[LESSON_DURATION];
	if (restLen > 9 || restLen < 2) throw string("Illegal number of lesson per week.");
}
catch (nlohmann::json::exception ex) {
	errorHandler(string("LessonInfo::FromJsonStr: ") + ex.what());
}
catch (string ex) {
	errorHandler("LessonInfo::FromJsonStr: " + ex);
}
catch (exception ex) {
	errorHandler(string("LessonInfo::FromJsonStr: ") + ex.what());
}
catch (...) {
	errorHandler("LessonInfo::FromJsonStr: Something is wrong.");
}

/* Return a string of all items in the LessonInfo.
 * @retval: The string of the output.
*/
string LessonInfo::DebugOutput() const noexcept
{
	string res;
	res = "Lesson ID: " + to_string(classID) + "\n";
	res += "Teacher ID: " + to_string(teacherID) + "\n";
	res += "Lesson Duration: " + to_string(length) + "\n";
	res += "Lesson Time: " + classTime.toHexStr() + "\n";
	res += "Lesson Capacity: " + to_string(capacity) + "\n";
	res += "Exam Time: " + to_string(examTime) + "\n";
	res += "Position: " + to_string(position) + "\n";

	return res;
}

/* Return a string of items to modify (in the database) of LessonInfo.
 * @retval: The string of the output.
*/
string LessonInfo::ModifyOutput() const noexcept
{
	nlohmann::json output;
	string res;
	output[LESSON_ID] = classID;
	output[LESSON_TIME] = classTime.toStr();
	output[LESSON_EXAMTIME] = examTime;
	output[LESSON_POSITION] = position;
	res = output.dump();

	int pos = res.find(string(LESSON_TIME));
	pos += strlen(LESSON_TIME) + 1;
	int cnt = 2;
	while (cnt) {
		if (res[pos] == '"') {
			res[pos] = ' ';
			cnt--;
		}
		pos++;
	}

	return res;
}

/* Return the number of periods to be cut into.
   @retval: the number of periods to be cut into.
*/
int LessonInfo::GetLessonPeriods() const noexcept
{
	if (restLen < 4) return 1;
	if (restLen < 7) return 2;
	return 3;
}

/* Return the next period length.
   @retval: the next period's length.
*/
int LessonInfo::GetNextPeriodLen() const noexcept
{
	if (!restLen) return 0;
	return restLen > 4 ? 3 : 2;
}

/* Cut down the period length to be scheduled.
*/
void LessonInfo::CutPeriodLen() noexcept
{
	if (restLen > 4) restLen -= 3;
	else restLen -= 2;
}

/* Initialize RoomInfo from a json file.
 * @param:
 * pathStr: The path of the json file.
 * @retval: none
 * Note: The file cannot be longer than 500 chars by default.
 * In this case, you should change the value of MAXJSONLEN.
*/
void RoomInfo::FromJsonFile(string pathStr)
try {
	ifstream fin(pathStr, fstream::in);
	if (!fin.is_open()) throw string("Failed opening input file. Is the filename or path wrong?");
	char jsonFile[JSONLENONCE + 1] = { 0 };
	string jsonStr;
	fin.read(jsonFile, JSONLENONCE);
	while (!fin.eof()) {
		jsonStr += jsonFile;
		fin.read(jsonFile, JSONLENONCE);
	}
	//	if (!fin.eof()) throw string("Expected EOF not met. Is json file longer than MAXJSONLEN?");
	jsonStr += jsonFile;
	fin.close();

	FromJsonStr(jsonStr);
}
catch (string ex) {
	errorHandler("RoomInfo::FromJsonFile: " + ex);
}
catch (exception ex) {
	errorHandler(string("RoomInfo::FromJsonFile: ") + ex.what());
}
catch (...) {
	errorHandler("RoomInfo::FromJsonFile: Something is wrong.");
}

/* Initialize RoomInfo from a json string.
 * @param:
 * jsonStr: The json string.
 * @retval: none
 * Note: The file cannot be longer than 500 chars by default.
 * In this case, you should change the value of MAXJSONLEN.
*/
void RoomInfo::FromJsonStr(string jsonStr)
try {
	int pos1, pos2;
	pos1 = jsonStr.find(ROOM_TIME) + strlen(ROOM_TIME) + 1;
	if (pos1 == string::npos) throw "Expected " + string(ROOM_TIME) + " not found. Is the json string complete?";
	while (pos1<jsonStr.length() && (jsonStr[pos1] > '9' || jsonStr[pos1] < '0')) pos1++;
	if (pos1 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");
	pos2 = pos1;
	while (pos2<jsonStr.length() && (jsonStr[pos2] >= '0' && jsonStr[pos2] <= '9')) pos2++;
	if (pos2 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");

	useTime = jsonStr.substr(pos1, pos2 - pos1);
	for (int i = pos1; i < pos2 - 1; ++i) jsonStr[i] = ' ';

	nlohmann::json streamIn;
	streamIn = nlohmann::json::parse(jsonStr);

	roomID = streamIn[ROOM_ID];
	capacity = streamIn[ROOM_CAPACITY];
}
catch (nlohmann::json::exception ex) {
	errorHandler(string("RoomInfo::FromJsonStr: ") + ex.what());
}
catch (string ex) {
	errorHandler("RoomInfo::FromJsonStr: " + ex);
}
catch (exception ex) {
	errorHandler(string("RoomInfo::FromJsonStr: ") + ex.what());
}
catch (...) {
	errorHandler("RoomInfo::FromJsonStr: Something is wrong.");
}

/* Return a string of all items in the RoomInfo.
 * @retval: The string of the output.
*/
string RoomInfo::DebugOutput() const noexcept
{
	string res;
	res += "Room ID: " + to_string(roomID) + "\n";
	res += "Room Capacity: " + to_string(capacity) + "\n";
	res += "Room Time: " + useTime.toStr() + "\n";
	return res;
}

/* Return a string of items to modify (in the database) of RoomInfo.
 * @retval: The string of the output.
*/
string RoomInfo::ModifyOutput() const noexcept
{
	nlohmann::json output;
	string res;
	output[ROOM_ID] = roomID;
	output[ROOM_TIME] = useTime.toStr();

	res = output.dump();

	int pos = res.find(string(ROOM_TIME));
	pos += strlen(ROOM_TIME) + 1;
	int cnt = 2;
	while (cnt) {
		if (res[pos] == '"') {
			res[pos] = ' ';
			cnt--;
		}
		pos++;
	}

	return res;
}

/* Use to compare two rooms when using sort().
*/
bool SortRoomPtr(const RoomPtr& p1, const RoomPtr& p2)
{
	return p1->GetCapacity() < p2->GetCapacity();
}

/* Initialize TeacherInfo from a json file.
*/
void TeacherInfo::FromJsonFile(string pathStr)
try {
	ifstream fin(pathStr, fstream::in);
	if (!fin.is_open()) throw string("Failed opening input file. Is the filename or path wrong?");
	char jsonFile[JSONLENONCE + 1] = { 0 };
	string jsonStr;
	fin.read(jsonFile, JSONLENONCE);
	while (!fin.eof()) {
		jsonStr += jsonFile;
		fin.read(jsonFile, JSONLENONCE);
	}
	//	if (!fin.eof()) throw string("Expected EOF not met. Is json file longer than MAXJSONLEN?");
	jsonStr += jsonFile;
	fin.close();

	FromJsonStr(jsonStr);
}
catch (string ex) {
	errorHandler("TeacherInfo::FromJsonFile: " + ex);
}
catch (exception ex) {
	errorHandler(string("TeacherInfo::FromJsonFile: ") + ex.what());
}
catch (...) {
	errorHandler("TeacherInfo::FromJsonFile: Something is wrong.");
}

/* Initialize TeacherInfo from a json string.
*/
void TeacherInfo::FromJsonStr(string jsonStr)
try {
	int pos1, pos2;
	pos1 = jsonStr.find(TEACHER_TIME) + strlen(TEACHER_TIME) + 1;
	if (pos1 == string::npos) throw "Expected " + string(TEACHER_TIME) + " not found. Is the json string complete?";
	while (pos1 < jsonStr.length() && (jsonStr[pos1] > '9' || jsonStr[pos1] < '0')) pos1++;
	if (pos1 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");
	pos2 = pos1;
	while (pos2 < jsonStr.length() && (jsonStr[pos2] >= '0' && jsonStr[pos2] <= '9')) pos2++;
	if (pos2 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");

	busyTime = jsonStr.substr(pos1, pos2 - pos1);
	for (int i = pos1; i < pos2 - 1; ++i) jsonStr[i] = ' ';

	nlohmann::json streamIn;
	streamIn = nlohmann::json::parse(jsonStr);

	ID = streamIn[TEACHER_ID];
}
catch (nlohmann::json::exception ex) {
	errorHandler(string("TeacherInfo::FromJsonStr: ") + ex.what());
}
catch (string ex) {
	errorHandler("TeacherInfo::FromJsonStr: " + ex);
}
catch (exception ex) {
	errorHandler(string("TeacherInfo::FromJsonStr: ") + ex.what());
}
catch (...) {
	errorHandler("TeacherInfo::FromJsonStr: Something is wrong.");
}

/* Add a lesson to the teacher.
 * @param:
 * lesson: pointer to the lesson.
 * @retval: none
 */
void TeacherInfo::AddLesson(LessonPtr lesson)
try {
	if (!lesson) throw string("Null lesson ptr.");
	lessons.push_back(lesson);
	lessonCnt += lesson->GetLessonPeriods();
}
catch (string ex) {
	errorHandler("TeacherInfo::AddLesson: " + ex);
}
catch (exception ex) {
	errorHandler(string("TeacherInfo::AddLesson: ") + ex.what());
}
catch (...) {
	errorHandler("TeacherInfo::AddLesson: Something is wrong.");
}

/* Printing shcedule. Only for debug.
*/
string TeacherInfo::OutputSchedule() const noexcept
{
	string res;
	string schedule[8][14];
	for (int i = 0; i < lessons.size(); ++i) {
		uint128 ltime = lessons[i]->GetClassTime();
		for (int j = 1; j <= 7; ++j) {
			uint128 dtime = ltime & 0x1FFFUi64;
			int p = dtime.getLast1();
			while (p != -1) {
				schedule[j][p + 1] = to_string(lessons[i]->GetClassID()) + "|" + to_string(lessons[i]->GetPosition()) + " ";
				if (schedule[j][p + 1].length() < 4) schedule[j][p + 1] += "\t\t";
				else if (schedule[j][p + 1].length() < 8) schedule[j][p + 1] += "\t";
				dtime = dtime & ~(0x1Ui64 << p);
				p = dtime.getLast1();
			}
			ltime = ltime >> 13;
		}
	}
	res = "Teacher: " + to_string(ID) + ":\n";
	res += "\t[ 一\t][ 二\t][ 三\t][ 四\t][ 五\t][ 六\t][ 日\t]\n";
	for (int i = 1; i <= 13; ++i) {
		res = res + to_string(i) + "\t";
		for (int j = 1; j <= 7; ++j) res += schedule[j][i] == "" ? "[\t]" : "[" + schedule[j][i] + "]";
		res += "\n";
	}
	return res;
}

/* Get the modify json file for database.
*/
string TeacherInfo::ModifyOutput()
{
	nlohmann::json output;
	string res;
	output[TEACHER_ID] = ID;
	output[TEACHER_TIME] = busyTime.toStr();

	res = output.dump();

	int pos = res.find(string(TEACHER_TIME));
	pos += strlen(TEACHER_TIME) + 1;
	int cnt = 2;
	while (cnt) {
		if (res[pos] == '"') {
			res[pos] = ' ';
			cnt--;
		}
		pos++;
	}

	return res;
}

/* Use to compare two teachers when using sort().
*/
bool SortTeacherPtr(const TeacherPtr& p1, const TeacherPtr& p2)
{
	return p1->GetLessonCount() > p2->GetLessonCount();
}

/* Initialize RoomList from a json file.
 * @param:
 * pathStr: The path of the json file.
 * @retval: none
 * Note: The file cannot be longer than 500 chars by default.
 * In this case, you should change the value of MAXJSONLEN.
 * Note that this is an overwriting process.
*/
void RoomList::FromJsonFile(string pathStr)
try {
	ifstream fin(pathStr, fstream::in);
	if (!fin.is_open()) throw string("Failed opening input file. Is the filename or path wrong?");
	char jsonFile[JSONLENONCE + 1] = { 0 };
	string jsonStr;
	fin.read(jsonFile, JSONLENONCE);
	while (!fin.eof()) {
		jsonStr += jsonFile;
		fin.read(jsonFile, JSONLENONCE);
	}
	//	if (!fin.eof()) throw string("Expected EOF not met. Is json file longer than MAXJSONLEN?");
	jsonStr += jsonFile;
	fin.close();

	FromJsonStr(jsonStr);
}
catch (string ex) {
	errorHandler("RoomList::FromJsonFile: " + ex);
}
catch (exception ex) {
	errorHandler(string("RoomList::FromJsonFile: ") + ex.what());
}
catch (...) {
	errorHandler("RoomList::FromJsonFile: Something is wrong.");
}

/* Initialize RoomList from a json string.
 * @param:
 * jsonStr: The json string.
 * The json string should begin with [ and end with ].
 * @retval: none
 * Note: The file cannot be longer than 500 chars by default.
 * In this case, you should change the value of MAXJSONLEN.
 * Note that this is an overwriting process.
*/
void RoomList::FromJsonStr(string jsonStr)
try {
	memset(useCount, 0, sizeof(useCount));
	rooms.clear();
	int pos = jsonStr.find_first_not_of(SPACECHARS);
	if (pos == string::npos) throw string("Unexpected EOF.");
	if (jsonStr[pos] != '[') throw string("Unexpected character '") + jsonStr[pos] + "'.";

	int lpos, rpos;

	pos = jsonStr.find_first_not_of(SPACECHARS, pos + 1);
	while (pos != string::npos && jsonStr[pos] != ']') {
		if (jsonStr[pos] == ',') {
			if(!rooms.size()) throw string("Unexpected character '") + jsonStr[pos] + "'.";
			else {
				pos = jsonStr.find_first_not_of(SPACECHARS, pos + 1);
				if (pos == string::npos) throw string("Unexpected EOF.");
			}
		}
		if (jsonStr[pos] != '{') throw string("Unexpected character '") + jsonStr[pos] + "'.";
		lpos = pos;
		int bcount = 1;
		while (bcount) {
			pos = jsonStr.find_first_of("{}", pos + 1);
			if (pos == string::npos)  throw string("Unexpected EOF.");
			if (jsonStr[pos] == '{') bcount++;
			else bcount--;
		}
		rpos = pos;
		RoomPtr tmpRoom = make_shared<RoomInfo>();
		tmpRoom->FromJsonStr(jsonStr.substr(lpos, rpos - lpos + 1));
		rooms.push_back(tmpRoom);

		pos = jsonStr.find_first_not_of(SPACECHARS, pos + 1);
	}
	if (pos == string::npos) throw string("Unexpected EOF.");
}
catch (nlohmann::json::exception ex) {
	errorHandler(string("RoomList::FromJsonStr: ") + ex.what());
}
catch (string ex) {
	errorHandler("RoomList::FromJsonStr: " + ex);
}
catch (exception ex) {
	errorHandler(string("RoomList::FromJsonStr: ") + ex.what());
}
catch (...) {
	errorHandler("RoomList::FromJsonStr: Something is wrong.");
}

/* Get the modify json for database.
*/
string RoomList::GetRoomModify() const
{
	string res = "[";
	for (int i = 0; i < rooms.size(); ++i) {
		if (i != 0) res += ",";
		res += rooms[i]->ModifyOutput();
	}
	res += "]";
	return res;
}

/* Allocate a room and time for a teacher and his class.
	@retval:
	LessonPtr is NULL when fail.
	LessonPtr->classTime is the time allocated.
	LessonPtr->position is the room allocated.

	Note thsi function will automaticly make the time allocated occupied.
*/
LessonPtr RoomList::AllocRoomFor(uint128 busytime, int totalLen, int cap)
try {
	if (cap > rooms[rooms.size() - 1]->GetCapacity()) throw string("The capacity required larger than the largest room.");
	uint128 prioMask, tmpMask;
	uint128 canAllocTime;

	uint128 allocRes;
	for (int i = 0; i <= 6; ++i) {
		prioMask = GetTimeMask(i, busytime);
		tmpMask = ~busytime & prioMask;
		for (int j = 0; j < rooms.size(); ++j) {
			if (rooms[j]->GetCapacity() < cap) continue;
			canAllocTime = ~rooms[j]->GetUseTime() & tmpMask;
			allocRes = FitInto(canAllocTime, totalLen, i == 6);
			if (allocRes) {
				LessonPtr res = make_shared<LessonInfo>();
				res->SetClassTime(allocRes);
				res->SetPosition(rooms[j]->GetRoomID());

				rooms[j]->AddUse(allocRes);

				for (int k = 1; k <= 7; ++k) {
					useCount[k] += (allocRes & uint128(0x545Ui64)).count1s();
					allocRes = allocRes >> 13;
				}

				return res;
			}
		}
	}
	return NULL;
}
catch (string ex) {
	errorHandler("RoomList::AllocRoomFor: " + ex);
	return NULL;
}
catch (exception ex) {
	errorHandler(string("RoomList::AllocRoomFor: ") + ex.what());
	return NULL;
}
catch (...) {
	errorHandler("RoomList::AllocRoomFor: Something is wrong.");
	return NULL;
}

/* Get time mask for allocating room.
	This mask ensures that the schedule is basically balanced.
*/
uint128 RoomList::GetTimeMask(int level, uint128 busytime) const noexcept
{
	int max1 = -1, pos1 = -1, max2 = -1, pos2 = -1, max3 = -1, pos3 = -1;
	for (int i = 1; i <= 7; ++i) {
		if (useCount[i] >= max1) {
			max3 = max2; pos3 = pos2;
			max2 = max1; pos2 = pos1;
			max1 = useCount[i]; pos1 = i;
		}
		else if (useCount[i] >= max2) {
			max3 = max2; pos3 = pos2;
			max2 = useCount[i]; pos2 = i;
		}
		else if (useCount[i] >= max3) {
			max3 = useCount[i]; pos3 = i;
		}
	}

	int m1 = -1, p1 = -1, m2 = -1, p2 = -1, m3 = -1, p3 = -1;
	uint128 tmpMask;
	int n;
	tmpMask = 0x545Ui64;
	for (int i = 1; i <= 7; ++i) {
		n = (busytime & tmpMask).count1s();
		if (n >= m1) {
			m3 = m2; p3 = p2;
			m2 = m1; p2 = p1;
			m1 = n; p1 = i;
		}
		else if (n >= m2) {
			m3 = m2; p3 = p2;
			m2 = n; p2 = i;
		}
		else if (n >= m3) {
			m3 = n; p3 = i;
		}
		tmpMask = tmpMask << 13;
	}

	uint128 res;
	int endDay = 5;

	switch (level)
	{
	case 0:
		for (int i = endDay; i >= 1; --i) {
			res = res << 13;
			if (i != pos1 && i != pos2 && i != pos3 && i != p1 && i != p2 && i != p3) res = res | 0x3FFUi64;
		}
		break;
	case 1:
		for (int i = endDay; i >= 1; --i) {
			res = res << 13;
			if (i != pos1 && i != pos2 && i != p1 && i != p2) res = res | 0x3FFUi64;
		}
		break;
	case 2:
		for (int i = endDay; i >= 1; --i) {
			res = res << 13;
			if (i != pos1 && i != p1) res = res | 0x3FFUi64;
		}
		break;
	case 3:
		if (m1 > rooms.size() / 2) if (rand() % rooms.size() < m1) endDay = 7;
		for (int i = endDay; i >= 1; --i) res = (res << 13) | 0x3FFUi64;
		break;
	case 4:
		if (m1 > rooms.size() / 2) if (rand() % rooms.size() < m1) endDay = 7;
		for (int i = endDay; i >= 1; --i) res = (res << 13) | 0x1FFFUi64;
		break;
	default:
		for (int i = 7; i >= 1; --i) res = (res << 13) | 0x1FFFUi64;
		break;
	}
	return res;
}

/* Try to put the lesson into a room's spare time with mask.
	@retval:
	0 when fail, lesson time when success.
*/
uint128 RoomList::FitInto(uint128 possibleTime, int totalLen, bool relaxed) const noexcept
{
	int count3 = 0, count2 = 0;
	int slot3 = 0, slot2 = 0;
	while (totalLen) {
		if (totalLen >= 5 || totalLen == 3) {
			totalLen -= 3;
			count3++;
		}
		else {
			totalLen -= 2;
			count2++;
		}
	}

	uint128 tmpMask;
	uint128 slotPos2, slotPos3;
	for (int i = 1; i <= 7; ++i) tmpMask = (tmpMask << 13) | 0x848Ui64;
	slotPos3 = possibleTime & tmpMask;
	slot3 = slotPos3.count1s();
	if (slot3 < count3) return 0;
	tmpMask = 0;
	for (int i = 1; i <= 7; ++i) tmpMask = (tmpMask << 13) | 0x101Ui64;
	slotPos2 = possibleTime & tmpMask;
	slot2 = slotPos2.count1s();

	uint128 res;
	uint128 tmpRes;
	if (relaxed) {
		if (slot2 + slot3 - count3 < count2) return 0;
		int i;
		for (i = 0; i < count2; ++i) {
			int p = slotPos2.getLast1();
			if (p == -1) break;
			tmpRes = 0x3;
			tmpRes = tmpRes << p;
			res = res | tmpRes;
			slotPos2 = slotPos2 & ~tmpRes;
		}
		for (; i < count2; ++i) {
			int p = slotPos3.getLast1();
			tmpRes = 0x3;
			tmpRes = tmpRes << (p % 13 == 6 ? p : p - 1);
			res = res | tmpRes;
			slotPos3 = slotPos3 & ~tmpRes;
		}
		for (i = 0; i < count3; ++i) {
			int p = slotPos3.getLast1();
			tmpRes = 0x7;
			tmpRes = tmpRes << (p - 1);
			res = res | tmpRes;
			slotPos3 = slotPos3 & ~tmpRes;
		}
	}
	else {
		if (slot2 < count2) return 0;
		for (int i = 0; i < count2; ++i) {
			int p = slotPos2.getLast1();
			tmpRes = 0x3;
			tmpRes = tmpRes << p;
			res = res | tmpRes;
			slotPos2 = slotPos2 & ~tmpRes;
		}
		for (int i = 0; i < count3; ++i) {
			int p = slotPos3.getLast1();
			tmpRes = 0x7;
			tmpRes = tmpRes << (p - 1);
			res = res | tmpRes;
			slotPos3 = slotPos3 & ~tmpRes;
		}
	}
	return res;
}

/* Read rooms from a json file.
	@param:
	pathStr: the file path.
	Note that the json file should begin with [ and end with ].
	Note that this is an overwriting process.
*/
void Schedule::RoomsFromJsonFile(string pathStr)
{
	rooms.FromJsonFile(pathStr);
}

/* Read rooms from a json string.
	@param:
	pathStr: the file path.
	Note that the json string should begin with [ and end with ].
	Note that this is an overwriting process.
*/
void Schedule::RoomsFromJsonStr(string jsonStr)
{
	rooms.FromJsonStr(jsonStr);
}

/* Read lessons from a json File.
	@param:
	pathStr: the path of input json file.
	Note that this is an overwriting process.
*/
void Schedule::LessonsFromJsonFile(string pathStr)
try {
	ifstream fin(pathStr, fstream::in);
	if (!fin.is_open()) throw string("Failed opening input file. Is the filename or path wrong?");
	char jsonFile[JSONLENONCE + 1] = { 0 };
	string jsonStr;
	fin.read(jsonFile, JSONLENONCE);
	while (!fin.eof()) {
		jsonStr += jsonFile;
		fin.read(jsonFile, JSONLENONCE);
	}
	//	if (!fin.eof()) throw string("Expected EOF not met. Is json file longer than MAXJSONLEN?");
	jsonStr += jsonFile;
	fin.close();

	LessonsFromJsonStr(jsonStr);
}
catch (string ex) {
	errorHandler("Schedule::LessonsFromJsonFile: " + ex);
}
catch (exception ex) {
	errorHandler(string("Schedule::LessonsFromJsonFile: ") + ex.what());
}
catch (...) {
	errorHandler("Schedule::LessonsFromJsonFile: Something is wrong.");
}

void Schedule::LessonsFromJsonStr(string jsonStr)
try {
	teachers.clear();
	int pos = jsonStr.find_first_not_of(SPACECHARS);
	if (pos == string::npos) throw string("Unexpected EOF.");
	if (jsonStr[pos] != '[') throw string("Unexpected character '") + jsonStr[pos] + "'.";

	int lpos, rpos;

	pos = jsonStr.find_first_not_of(SPACECHARS, pos + 1);
	while (pos != string::npos && jsonStr[pos] != ']') {
		if (jsonStr[pos] == ',') {
			if (!teachers.size()) throw string("Unexpected character '") + jsonStr[pos] + "'.";
			else {
				pos = jsonStr.find_first_not_of(SPACECHARS, pos + 1);
				if(pos==string::npos) throw string("Unexpected EOF.");
			}
		}
		if (jsonStr[pos] != '{') throw string("Unexpected character '") + jsonStr[pos] + "'.";
		lpos = pos;
		int bcount = 1;
		while (bcount) {
			pos = jsonStr.find_first_of("{}", pos + 1);
			if (pos == string::npos)  throw string("Unexpected EOF.");
			if (jsonStr[pos] == '{') bcount++;
			else bcount--;
		}
		rpos = pos;
		LessonPtr tmpLesson = make_shared<LessonInfo>();
		tmpLesson->FromJsonStr(jsonStr.substr(lpos, rpos - lpos + 1));
		//		rooms.push_back(tmpRoom);
		int i;
		for (i = 0; i < teachers.size(); ++i) {
			if (teachers[i]->GetID() == tmpLesson->GetTeacherID()) break;
		}
		if (i == teachers.size()) {
			TeacherPtr tmpTeacher = make_shared<TeacherInfo>(tmpLesson->GetTeacherID());
			tmpTeacher->AddLesson(tmpLesson);
			teachers.push_back(tmpTeacher);
		}
		else {
			teachers[i]->AddLesson(tmpLesson);
		}
		pos = jsonStr.find_first_not_of(SPACECHARS, pos + 1);
	}
	if (pos == string::npos) throw string("Unexpected EOF.");
}
catch (nlohmann::json::exception ex) {
	errorHandler(string("Schedule::LessonsFromJsonStr: ") + ex.what());
}
catch (string ex) {
	errorHandler("Schedule::LessonsFromJsonStr: " + ex);
}
catch (exception ex) {
	errorHandler(string("Schedule::LessonsFromJsonStr: ") + ex.what());
}
catch (...) {
	errorHandler("Schedule::LessonsFromJsonStr: Something is wrong.");
}

/* Get the lessons modifies json for database.
*/
string Schedule::GetLessonModify() const
{
	string res = "[";
	for (int i = 0; i < teachers.size(); ++i) {
		for (int j = 0; j < teachers[i]->lessons.size(); ++j) {
			if (i != 0 || j != 0) res += ",";
			res += teachers[i]->lessons[j]->ModifyOutput();
		}
	}
	res += "]";
	return res;
}

/* Get the teacher modifies for database.
*/
string Schedule::GetTeacherModify() const
{
	string res = "[";
	for (int i = 0; i < teachers.size(); ++i) {
		if (i != 0) res += ",";
		res += teachers[i]->ModifyOutput();
	}
	res += "]";
	return res;
}

/* Scheduling.
*/
void Schedule::DoSchedule()
try {
	ResetForSched();
	sort(teachers.begin(), teachers.end(), SortTeacherPtr);
	rooms.Sort();

	LessonPtr tmpSche;
	TeacherPtr tmpTeacher;
	LessonPtr tmpLesson;
	for (int i = 0; i < teachers.size(); ++i) {
		tmpTeacher = teachers[i];
		for (int j = 0; j < tmpTeacher->lessons.size(); j++) {
			tmpLesson = tmpTeacher->lessons[j];
			tmpSche = rooms.AllocRoomFor(tmpTeacher->busyTime, tmpLesson->GetLength(), tmpLesson->GetCapacity());
			if (!tmpSche) {
				return;
			}
			tmpLesson->SetPosition(tmpSche->GetPosition());
			tmpLesson->SetClassTime(tmpSche->GetClassTime());
			tmpTeacher->busyTime = tmpTeacher->busyTime | tmpSche->GetClassTime();
		}
	}
	isSuccess = true;
}
catch (string ex) {
	errorHandler("Schedule::DoSchedule: " + ex);
}
catch (exception ex) {
	errorHandler(string("Schedule::DoSchedule: ") + ex.what());
}
catch (...) {
	errorHandler("Schedule::DoSchedule: Something is wrong.");
}

/* Print the schedule. For debug only.
*/
string Schedule::DebugOutput() const noexcept
{
	string res;
	for (int i = 0; i < teachers.size(); ++i) {
		res += teachers[i]->OutputSchedule() + "\n";
	}
	return res;
}

/* Write Output files, which includes whether success and the database to modify.
*/
void Schedule::OutputRes()
{
	nlohmann::json output;
	string outputStr;
	string roomModify, lessonModify, teacherModify;
	ofstream fout(stateOutputFile, fstream::out);
	if (!isSuccess) {
		output[STATE_SUCCESS] = false;
		output[STATE_INFO] = "Failed scheduling. If this continues happening, there may be too many lessons.";
		fout << output;
		fout.close();
		return;
	}
	output[STATE_SUCCESS] = true;
	output[STATE_INFO] = "Successed scheduling.";
	roomModify = rooms.GetRoomModify();
	output[STATE_ROOMS] = roomModify;
	lessonModify = GetLessonModify();
	output[STATE_LESSONS] = lessonModify;
	teacherModify = GetTeacherModify();
	output[STATE_TEACHERS] = teacherModify;
	outputStr = output.dump();

	outputStr.erase(remove(outputStr.begin(), outputStr.end(), '\\'), outputStr.end());
	
	int tmppos;
	tmppos = outputStr.find(roomModify);
	outputStr[tmppos - 1] = ' ';
	outputStr[tmppos + roomModify.length()] = ' ';

	tmppos = outputStr.find(lessonModify);
	outputStr[tmppos - 1] = ' ';
	outputStr[tmppos + lessonModify.length()] = ' ';

	tmppos = outputStr.find(teacherModify);
	outputStr[tmppos - 1] = ' ';
	outputStr[tmppos + teacherModify.length()] = ' ';

	fout << outputStr;
	fout.close();

}

/* Read target time from jsonfile.
*/
void Modify::TargetTimeFromJsonFile(string pathStr)
try {
	ifstream fin(pathStr, fstream::in);
	if (!fin.is_open()) throw string("Failed opening input file. Is the filename or path wrong?");
	char jsonFile[JSONLENONCE + 1] = { 0 };
	string jsonStr;
	fin.read(jsonFile, JSONLENONCE);
	while (!fin.eof()) {
		jsonStr += jsonFile;
		fin.read(jsonFile, JSONLENONCE);
	}
	jsonStr += jsonFile;
	fin.close();

	TargetTimeFromJsonStr(jsonStr);
}
catch (string ex) {
	errorHandler("Modify::FromJsonFile: " + ex);
}
catch (exception ex) {
	errorHandler(string("Modify::FromJsonFile: ") + ex.what());
}
catch (...) {
	errorHandler("Modify::FromJsonFile: Something is wrong.");
}

/* Read target time from a json string.
*/
void Modify::TargetTimeFromJsonStr(string jsonStr)
try {
	int pos1, pos2;
	pos1 = jsonStr.find(MODIFY_TIME) + strlen(MODIFY_TIME) + 1;
	if (pos1 == string::npos) throw "Expected " + string(MODIFY_TIME) + " not found. Is the json string complete?";
	while (pos1 < jsonStr.length() && (jsonStr[pos1] > '9' || jsonStr[pos1] < '0')) pos1++;
	if (pos1 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");
	pos2 = pos1;
	while (pos2 < jsonStr.length() && (jsonStr[pos2] >= '0' && jsonStr[pos2] <= '9')) pos2++;
	if (pos2 == jsonStr.length()) throw string("Unexpected end of string. Is the json string complete?");

	targetTime = jsonStr.substr(pos1, pos2 - pos1);
}
catch (string ex) {
	errorHandler("Modify::FromJsonStr: " + ex);
}
catch (exception ex) {
	errorHandler(string("Modify::FromJsonStr: ") + ex.what());
}
catch (...) {
	errorHandler("Modify::FromJsonStr: Something is wrong.");
}

/* Try to modify.
*/
void Modify::DoModify()
try {
	uint128 roomTime = room.GetUseTime();
	uint128 curLessonTime = lesson.GetClassTime();
	uint128 teacherTime = teacher.GetBusyTime();

	roomTime = roomTime & ~curLessonTime;
	teacherTime = teacherTime & ~curLessonTime;

	if (roomTime & targetTime) throw string("There are other lessons in the classroom on the target time.");
	if (teacherTime & targetTime) throw string("The teacher has other lessons on the target time.");
	if (curLessonTime.count1s() != targetTime.count1s()) throw string("The lesson's length should not be changed.");

	isSuccess = true;
	room.SetUsetTime(roomTime | targetTime);
	lesson.SetClassTime(targetTime);
	teacher.SetBusyTime(teacherTime | targetTime);
}
catch (string ex) {
	errorHandler("Modify::DoModify: " + ex);
}
catch (exception ex) {
	errorHandler(string("Modify::DoModify: ") + ex.what());
}
catch (...) {
	errorHandler("Modify::DoModify: Something is wrong.");
}

/* Output the result which includes the database to modify and whether the modify is success.
*/
void Modify::OutputRes()
{
	nlohmann::json output;
	string outputStr;
	string roomModify, lessonModify, teacherModify;
	ofstream fout(stateOutputFile, fstream::out);
	output[STATE_SUCCESS] = true;
	output[STATE_INFO] = "Successed modifying.";
	roomModify = room.ModifyOutput();
	output[STATE_SINGLEROOM] = roomModify;
	lessonModify = lesson.ModifyOutput();
	output[STATE_SINGLELESSON] = lessonModify;
	teacherModify = teacher.ModifyOutput();
	output[STATE_SINGLETEACHER] = teacherModify;
	outputStr = output.dump();

	outputStr.erase(remove(outputStr.begin(), outputStr.end(), '\\'), outputStr.end());

	int tmppos;
	tmppos = outputStr.find(roomModify);
	outputStr[tmppos - 1] = ' ';
	outputStr[tmppos + roomModify.length()] = ' ';

	tmppos = outputStr.find(lessonModify);
	outputStr[tmppos - 1] = ' ';
	outputStr[tmppos + lessonModify.length()] = ' ';

	tmppos = outputStr.find(teacherModify);
	outputStr[tmppos - 1] = ' ';
	outputStr[tmppos + teacherModify.length()] = ' ';

	fout << outputStr;
	fout.close();
}

/* An error handler.
 * Output the errorInfo and quit.
*/
void defaultErrorHandler(string errorInfo)
{
	ofstream fout(stateOutputFile, fstream::out);
	fout << "{\n\t\"" << STATE_SUCCESS << "\":false,\n\t\"" << STATE_INFO << "\":\"" << errorInfo << "\"\n}";
	fout.close();
	cout << errorInfo << endl;
//	system("pause");
	exit(0);
}