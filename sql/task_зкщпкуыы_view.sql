CREATE OR REPLACE VIEW task_progress
as
	select 
		now() > tasks.control_timestamp as overtime_control_time,
		tasks.id as task_id,
		cast(EXTRACT(EPOCH FROM age(tasks.control_timestamp, tasks.start_timestamp)) as int),
		cast(EXTRACT(EPOCH FROM age(now(), tasks.start_timestamp)) as int)
	from tasks