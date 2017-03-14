CREATE OR REPLACE FUNCTION task_timeranges(
	IN  taskid                                  integer, 
	OUT overtime_control_time                   boolean,
	OUT total_task_from_start_to_control_time   character varying, 
	OUT left_task_from_start_to_now_time        character varying, 
	OUT remaining_task_from_now_to_control_time character varying,
	OUT total_task_from_start_to_end_time       character varying, 
	OUT total_task_from_control_to_end_time     character varying)
AS
$$
declare
	dtformat varchar DEFAULT 'YY.MM.DD HH24:MI:SS';
begin
	select 
		now() > tasks.control_timestamp as r_overtime_control_time,
		to_char(age(tasks.control_timestamp, tasks.start_timestamp), 'YY.MM.DD HH24:MI:SS') as r_total_task_from_start_to_control_time,
		to_char(age(now()                  , tasks.start_timestamp), 'YY.MM.DD HH24:MI:SS') as r_left_task_from_start_to_now_time,
		(
			case when now() > tasks.control_timestamp
			then to_char(-age(tasks.control_timestamp,                 now()), 'YY.MM.DD HH24:MI:SS')
			else to_char( age(tasks.control_timestamp,                 now()), 'YY.MM.DD HH24:MI:SS')
			end
		) as r_remaining_task_from_now_to_control_time,
		(
			case when end_timestamp is null
			then null
			else (
				case when tasks.end_timestamp > tasks.control_timestamp
				then to_char(age(tasks.end_timestamp    ,  tasks.control_timestamp), 'YY.MM.DD HH24:MI:SS')
				else to_char(age(tasks.control_timestamp, tasks.end_timestamp     ), 'YY.MM.DD HH24:MI:SS')
				end
			     )
			end
		) as r_total_task_from_control_to_end_time,
		(		
			case when end_timestamp is null
			then null
			else to_char(age(tasks.end_timestamp,  tasks.start_timestamp), 'YY.MM.DD HH24:MI:SS')
			end
		) as r_total_task_from_start_to_end_time
	into 
		overtime_control_time, 
		total_task_from_start_to_control_time, 
		left_task_from_start_to_now_time,
		remaining_task_from_now_to_control_time,
		total_task_from_start_to_end_time,
		total_task_from_control_to_end_time
	from tasks
	where tasks.id = taskid;
end;
$$
  LANGUAGE plpgsql;
ALTER FUNCTION task_timeranges(integer)
  OWNER TO docflow;