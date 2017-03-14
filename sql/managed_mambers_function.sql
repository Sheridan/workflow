CREATE or replace FUNCTION managed_divisions
	(in memberID integer, 
	 out id integer,
	 out name character varying(128),
	 out short_name character varying(32),
	 out parent_division integer,
	 out manager_member_id integer) RETURNS SETOF RECORD
AS 
$managed_divisions$
	WITH RECURSIVE r_divisions(id, name, short_name, parent_division, manager_member_id) AS
	(
		select 	divisions.id, 
			divisions.name, 
			divisions.short_name, 
			divisions.parent_division, 
			divisions.manager_member_id 
			from divisions 
			where divisions.manager_member_id=memberID 
                UNION ALL
		select 
			d.id, 
			d.name, 
			d.short_name, 
			d.parent_division, 
			d.manager_member_id 
		from r_divisions rd, divisions d where d.id=rd.parent_division
	)
SELECT 
	r_divisions.id, 
	r_divisions.name, 
	r_divisions.short_name, 
	r_divisions.parent_division, 
	r_divisions.manager_member_id  
FROM r_divisions
$managed_divisions$
LANGUAGE SQL;