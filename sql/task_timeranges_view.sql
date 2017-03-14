CREATE OR REPLACE VIEW task_timeranges
as
	select 
		now() > tasks.control_timestamp as overtime_control_time,
		to_char(age(tasks.control_timestamp, tasks.start_timestamp), 'YY|MM|DD|HH24|MI|SS') as total_task_from_start_to_control_time,
		to_char(age(now()                  , tasks.start_timestamp), 'YY|MM|DD|HH24|MI|SS') as left_task_from_start_to_now_time,
		(
			case when now() > tasks.control_timestamp
			then to_char(-age(tasks.control_timestamp,                 now()), 'YY|MM|DD|HH24|MI|SS')
			else to_char( age(tasks.control_timestamp,                 now()), 'YY|MM|DD|HH24|MI|SS')
			end
		) as remaining_task_from_now_to_control_time,
		(
			case when end_timestamp is null
			then null
			else (
				case when tasks.end_timestamp > tasks.control_timestamp
				then to_char(age(tasks.end_timestamp    ,  tasks.control_timestamp), 'YY|MM|DD|HH24|MI|SS')
				else to_char(age(tasks.control_timestamp, tasks.end_timestamp     ), 'YY|MM|DD|HH24|MI|SS')
				end
			     )
			end
		) as total_task_from_control_to_end_time,
		(		
			case when end_timestamp is null
			then null
			else to_char(age(tasks.end_timestamp,  tasks.start_timestamp), 'YY|MM|DD|HH24|MI|SS')
			end
		) as total_task_from_start_to_end_time,
		tasks.id as task_id
	from tasks