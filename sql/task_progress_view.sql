CREATE OR REPLACE VIEW task_progress_statuses
as
	select 

		task_progress_in_percents.task_id as task_id,
		(
			case task_progress_in_percents.overdue_flag
			when true then
				'overdue'
			else
				case 
				when task_progress_in_percents.percents_done >=0 and 
			             task_progress_in_percents.percents_done < 70
					then 'normal'
				when task_progress_in_percents.percents_done >=70 and 
				     task_progress_in_percents.percents_done < 90
					then 'warning'
				when task_progress_in_percents.percents_done >=90 and 
				     task_progress_in_percents.percents_done <=100
					then 'critical'
				end
			end
		) as task_done_status,
		task_progress_in_percents.task_id as task_id,
		(
			case task_progress_in_percents.overdue_flag
			when true then
				'Нарушен срок'
			else
				case 
				when task_progress_in_percents.percents_done >=0 and 
			             task_progress_in_percents.percents_done < 70
					then 'Время работ не достигло 70% от общего времени выполнения'
				when task_progress_in_percents.percents_done >=70 and 
				     task_progress_in_percents.percents_done < 90
					then 'Время работ между 70% и 90% от общего времени выполнения'
				when task_progress_in_percents.percents_done >=90 and 
				     task_progress_in_percents.percents_done <=100
					then 'Время работ более 90% от общего времени выполнения'
				end
			end
		) as task_done_status
	from task_progress_in_percents