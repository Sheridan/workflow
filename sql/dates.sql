
select 
	tasks.caption,
	to_char(tasks.start_timestamp, 'YY.MM.DD HH24:MI:SS') as start_ts,
	to_char(tasks.control_timestamp, 'YY.MM.DD HH24:MI:SS') as ctrl_ts,
	to_char(age(tasks.control_timestamp, tasks.start_timestamp), 'YY.MM.DD HH24:MI:SS') as totlal_time,
	to_char(age(now(), tasks.start_timestamp), 'YY.MM.DD HH24:MI:SS')  as time_left,
	to_char(age(tasks.control_timestamp, now()), 'YY.MM.DD HH24:MI:SS') as time_to,
	tasks.control_timestamp > now() as dd
from tasks;
