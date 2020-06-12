# Schedule module

## Note
**You can always change the input and output json's key by modifying macros in Schedule.h and recompile the program.**

There are input examples in "./example jsons".
## Auto scheduling
Use command

	*.exe autosched

This requrires the following input files:

- **autosched_lessons.json**, which contains a json array of all lessons' infomation.
- **autosched_rooms.json**, which contains a json array of all rooms' infomation.

And this will produce an output file named **state.json**.

This file contains whether the auto scheduling is success and the error infomation when the program fails.

When the auto scheduling succeeds, the file will also contain three arrays, which are named by default "rooms", "lessons" and "teachers". These three arraies includes the data which database needs to update.

## Modify

Use command

	*.exe modify

This requires the following input files:

- modify_lesson.json, which contains the data of lesson to modify.
- modify_room.json, which contains the data of the room which the lesson is located.
- modify_time.json, which contains the time to change into.
- modify_teacher.json, which contains the data of the lesson's teacher.

This will also produce an output file named **state.json**.

This file contains whether the modify is successful and the error infomation when the program fails.

When the modify succeeds, the file will also contain three objects, which are named by default "room", "lesson" and "teacher". **Note this is different from the output of auto scheduling, whose key is in plural form.** These three objects includes the data which database needs to update.