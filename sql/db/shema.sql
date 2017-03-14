--
-- PostgreSQL database dump
--

-- Dumped from database version 9.5.2
-- Dumped by pg_dump version 9.5.5

-- Started on 2017-03-14 10:02:51 MSK

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

DROP DATABASE docflow;
--
-- TOC entry 2271 (class 1262 OID 16403)
-- Name: docflow; Type: DATABASE; Schema: -; Owner: docflow
--

CREATE DATABASE docflow WITH TEMPLATE = template0 ENCODING = 'UTF8' LC_COLLATE = 'ru_RU.UTF-8' LC_CTYPE = 'ru_RU.UTF-8';


ALTER DATABASE docflow OWNER TO docflow;

\connect docflow

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- TOC entry 1 (class 3079 OID 12371)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2274 (class 0 OID 0)
-- Dependencies: 1
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 575 (class 1247 OID 24895)
-- Name: changelog_types; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE changelog_types AS ENUM (
    'control_date_changed',
    'comment_added',
    'opened',
    'closed',
    'control_member_changed',
    'performer_member_changed',
    'owner_member_changed',
    'document_added',
    'description_edited',
    'member_created',
    'priority_changed',
    'member_edited',
    'member_right_changed',
    'division_edited',
    'division_created',
    'file_uploaded',
    'caption_edited',
    'division_changed',
    'coworker_removed',
    'coworker_added'
);


ALTER TYPE changelog_types OWNER TO postgres;

--
-- TOC entry 578 (class 1247 OID 24933)
-- Name: objects_classes; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE objects_classes AS ENUM (
    'task',
    'member',
    'division'
);


ALTER TYPE objects_classes OWNER TO postgres;

--
-- TOC entry 581 (class 1247 OID 24941)
-- Name: rights_types; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE rights_types AS ENUM (
    'can_edit',
    'can_delete',
    'can_add',
    'can_work_as_division_manager',
    'admin'
);


ALTER TYPE rights_types OWNER TO postgres;

--
-- TOC entry 584 (class 1247 OID 24866)
-- Name: task_priorityes; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE task_priorityes AS ENUM (
    'high',
    'default',
    'low'
);


ALTER TYPE task_priorityes OWNER TO postgres;

--
-- TOC entry 587 (class 1247 OID 24861)
-- Name: task_statuses; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE task_statuses AS ENUM (
    'open',
    'closed',
    'not_assigned'
);


ALTER TYPE task_statuses OWNER TO postgres;

--
-- TOC entry 216 (class 1255 OID 16634)
-- Name: changelog_type_to_name(changelog_types); Type: FUNCTION; Schema: public; Owner: docflow
--

CREATE FUNCTION changelog_type_to_name(changelog_type changelog_types) RETURNS character varying
    LANGUAGE plpgsql
    AS $$
begin
	case changelog_type
		when 'control_date_changed'     then return 'Изменена дата контроля';
		when 'comment_added'            then return 'Добавлен комментарий';
		when 'opened'                   then return 'Открыто';
		when 'closed'                   then return 'Закрыто';
		when 'control_member_changed'   then return 'Изменен контроллёр';
		when 'performer_member_changed' then return 'Изменен исполнитель';
		when 'owner_member_changed'     then return 'Изменен владелец';
		when 'document_added'           then return 'Добавлен документ';
		when 'description_edited'       then return 'Изменено описание';
		when 'member_created'           then return 'Создан пользователь';
		when 'priority_changed'         then return 'Изменён приоритет';
		when 'member_edited'            then return 'Отредактирован пользователь';
		when 'member_right_changed'     then return 'Изменены права пользователя';
		when 'division_created'         then return 'Создано подразделение';
		when 'division_edited'          then return 'Изменено подразделение';
		when 'file_uploaded'            then return 'Загружен файл';
		when 'caption_edited'           then return 'Изменён заголовок';
		when 'division_changed'         then return 'Изменено целевое подразделение';
		when 'coworker_added'           then return 'Добавлен исполнитель в рабочую группу';
		when 'coworker_removed'         then return 'Исполнитель удален из рабочей группы';
	end case;
	return concat('Функция changelog_type_to_name требует исправлений. Тип: ', changelog_type);
end
$$;


ALTER FUNCTION public.changelog_type_to_name(changelog_type changelog_types) OWNER TO docflow;

--
-- TOC entry 217 (class 1255 OID 16635)
-- Name: managed_divisions(integer); Type: FUNCTION; Schema: public; Owner: docflow
--

CREATE FUNCTION managed_divisions(memberid integer, OUT id integer, OUT name character varying, OUT short_name character varying, OUT parent_division integer, OUT manager_member_id integer, OUT division_level integer) RETURNS SETOF record
    LANGUAGE sql
    AS $$
	WITH RECURSIVE r_divisions(id, name, short_name, parent_division, manager_member_id, division_level) AS
	(
		select 	divisions.id, 
			divisions.name, 
			divisions.short_name, 
			divisions.parent_division, 
			divisions.manager_member_id,
			1 as division_level
			from divisions 
			where divisions.manager_member_id=memberID 
                UNION ALL
		select 
			d.id, 
			d.name, 
			d.short_name, 
			d.parent_division, 
			d.manager_member_id,
			rd.division_level + 1 as division_level
		from r_divisions rd, divisions d where rd.id=d.parent_division
	)
	SELECT 
		r_divisions.id, 
		r_divisions.name, 
		r_divisions.short_name, 
		r_divisions.parent_division, 
		r_divisions.manager_member_id,
		r_divisions.division_level
	FROM r_divisions
$$;


ALTER FUNCTION public.managed_divisions(memberid integer, OUT id integer, OUT name character varying, OUT short_name character varying, OUT parent_division integer, OUT manager_member_id integer, OUT division_level integer) OWNER TO docflow;

--
-- TOC entry 218 (class 1255 OID 16636)
-- Name: objects_classes_to_name(objects_classes); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION objects_classes_to_name(obj objects_classes) RETURNS character varying
    LANGUAGE plpgsql
    AS $$
begin
	case obj
		when 'task'     then return 'Задача';
		when 'member'   then return 'Пользователь';
		when 'division' then return 'Подразделение';
	end case;
	return concat('Функция objects_classes_to_name требует исправлений. Тип: ', obj);
end
$$;


ALTER FUNCTION public.objects_classes_to_name(obj objects_classes) OWNER TO postgres;

--
-- TOC entry 219 (class 1255 OID 16637)
-- Name: rights_types_to_name(rights_types); Type: FUNCTION; Schema: public; Owner: docflow
--

CREATE FUNCTION rights_types_to_name(rights rights_types) RETURNS character varying
    LANGUAGE plpgsql
    AS $$
begin
	case rights
		when 'can_edit'                     then return 'Редактирование';
		when 'can_delete'                   then return 'Удаление';
		when 'can_add'                      then return 'Создание';
		when 'can_work_as_division_manager' then return 'Права руководителя подразделения';
		when 'admin'                        then return 'Администратор';
	end case;
	return concat('Функция rights_types_to_name требует исправлений. Тип: ', rights);
end
$$;


ALTER FUNCTION public.rights_types_to_name(rights rights_types) OWNER TO docflow;

--
-- TOC entry 220 (class 1255 OID 16638)
-- Name: task_priorityes_to_name(task_priorityes); Type: FUNCTION; Schema: public; Owner: docflow
--

CREATE FUNCTION task_priorityes_to_name(priority task_priorityes) RETURNS character varying
    LANGUAGE plpgsql
    AS $$
begin
	case priority
		when 'high'     then return 'Высокий';
		when 'default'  then return 'Обычный';
		when 'low'      then return 'Низкий';
	end case;
	return concat('Функция task_priorityes_to_name требует исправлений. Тип: ', priority);
end
$$;


ALTER FUNCTION public.task_priorityes_to_name(priority task_priorityes) OWNER TO docflow;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 189 (class 1259 OID 24887)
-- Name: changelog; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE changelog (
    id integer NOT NULL,
    change_timestamp timestamp with time zone DEFAULT now(),
    what_changed changelog_types,
    class_id integer NOT NULL,
    changer_member_id integer NOT NULL,
    class_of_change objects_classes DEFAULT 'task'::objects_classes NOT NULL,
    description text
);


ALTER TABLE changelog OWNER TO docflow;

--
-- TOC entry 188 (class 1259 OID 24885)
-- Name: changelog_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE changelog_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE changelog_id_seq OWNER TO docflow;

--
-- TOC entry 2275 (class 0 OID 0)
-- Dependencies: 188
-- Name: changelog_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE changelog_id_seq OWNED BY changelog.id;


--
-- TOC entry 197 (class 1259 OID 33151)
-- Name: comments; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE comments (
    id integer NOT NULL,
    comment text NOT NULL,
    comment_timestamp timestamp with time zone DEFAULT now() NOT NULL,
    owner_id integer NOT NULL,
    object_class objects_classes NOT NULL,
    class_id integer NOT NULL
);


ALTER TABLE comments OWNER TO docflow;

--
-- TOC entry 196 (class 1259 OID 33149)
-- Name: comments_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE comments_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE comments_id_seq OWNER TO docflow;

--
-- TOC entry 2276 (class 0 OID 0)
-- Dependencies: 196
-- Name: comments_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE comments_id_seq OWNED BY comments.id;


--
-- TOC entry 184 (class 1259 OID 24826)
-- Name: divisions; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE divisions (
    id integer NOT NULL,
    name character varying(128),
    short_name character varying(32),
    parent_division integer DEFAULT 0 NOT NULL,
    manager_member_id integer
);


ALTER TABLE divisions OWNER TO docflow;

--
-- TOC entry 183 (class 1259 OID 24824)
-- Name: divisions_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE divisions_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE divisions_id_seq OWNER TO docflow;

--
-- TOC entry 2277 (class 0 OID 0)
-- Dependencies: 183
-- Name: divisions_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE divisions_id_seq OWNED BY divisions.id;


--
-- TOC entry 202 (class 1259 OID 33716)
-- Name: maintenance_work; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE maintenance_work (
    id integer NOT NULL,
    caption character varying(128) NOT NULL,
    description text,
    start_date timestamp with time zone DEFAULT now(),
    maintenance_work_class_id integer,
    stop_date timestamp with time zone
);


ALTER TABLE maintenance_work OWNER TO docflow;

--
-- TOC entry 2278 (class 0 OID 0)
-- Dependencies: 202
-- Name: COLUMN maintenance_work.stop_date; Type: COMMENT; Schema: public; Owner: docflow
--

COMMENT ON COLUMN maintenance_work.stop_date IS 'На случай, если в работах более нет необходимости';


--
-- TOC entry 200 (class 1259 OID 33705)
-- Name: maintenance_work_class; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE maintenance_work_class (
    id integer NOT NULL,
    caption character varying(128) NOT NULL,
    description text NOT NULL,
    interval_between_tasks interval NOT NULL,
    interval_task_runtime interval NOT NULL,
    target_division_id integer,
    target_controller_id integer
);


ALTER TABLE maintenance_work_class OWNER TO docflow;

--
-- TOC entry 199 (class 1259 OID 33703)
-- Name: maintenance_work_class_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE maintenance_work_class_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE maintenance_work_class_id_seq OWNER TO docflow;

--
-- TOC entry 2279 (class 0 OID 0)
-- Dependencies: 199
-- Name: maintenance_work_class_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE maintenance_work_class_id_seq OWNED BY maintenance_work_class.id;


--
-- TOC entry 201 (class 1259 OID 33714)
-- Name: maintenance_work_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE maintenance_work_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE maintenance_work_id_seq OWNER TO docflow;

--
-- TOC entry 2280 (class 0 OID 0)
-- Dependencies: 201
-- Name: maintenance_work_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE maintenance_work_id_seq OWNED BY maintenance_work.id;


--
-- TOC entry 203 (class 1259 OID 33726)
-- Name: maintenance_work_tasks; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE maintenance_work_tasks (
    maintenance_work_id integer NOT NULL,
    task_id integer NOT NULL
);


ALTER TABLE maintenance_work_tasks OWNER TO docflow;

--
-- TOC entry 182 (class 1259 OID 24765)
-- Name: members; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE members (
    id integer NOT NULL,
    name character varying(64),
    password character varying(128),
    email character varying(128),
    division_id integer,
    last_seen_timestamp timestamp with time zone DEFAULT now() NOT NULL,
    fired boolean DEFAULT false NOT NULL
);


ALTER TABLE members OWNER TO docflow;

--
-- TOC entry 181 (class 1259 OID 24763)
-- Name: members_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE members_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE members_id_seq OWNER TO docflow;

--
-- TOC entry 2281 (class 0 OID 0)
-- Dependencies: 181
-- Name: members_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE members_id_seq OWNED BY members.id;


--
-- TOC entry 185 (class 1259 OID 24843)
-- Name: members_rights; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE members_rights (
    member_id integer NOT NULL,
    allow rights_types NOT NULL,
    object_class objects_classes NOT NULL
);


ALTER TABLE members_rights OWNER TO docflow;

--
-- TOC entry 198 (class 1259 OID 33695)
-- Name: task_coworkers; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE task_coworkers (
    task_id integer NOT NULL,
    coworker_member_id integer NOT NULL
);


ALTER TABLE task_coworkers OWNER TO docflow;

--
-- TOC entry 187 (class 1259 OID 24851)
-- Name: tasks; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE tasks (
    id integer NOT NULL,
    caption character varying(128) NOT NULL,
    description text,
    target_division_id integer NOT NULL,
    start_timestamp timestamp with time zone DEFAULT now() NOT NULL,
    control_timestamp timestamp with time zone,
    end_timestamp timestamp with time zone,
    performer_member_id integer DEFAULT 0 NOT NULL,
    creator_member_id integer,
    controller_member_id integer DEFAULT 0 NOT NULL,
    status task_statuses DEFAULT 'not_assigned'::task_statuses NOT NULL,
    priority task_priorityes DEFAULT 'default'::task_priorityes NOT NULL
);


ALTER TABLE tasks OWNER TO docflow;

--
-- TOC entry 2282 (class 0 OID 0)
-- Dependencies: 187
-- Name: COLUMN tasks.performer_member_id; Type: COMMENT; Schema: public; Owner: docflow
--

COMMENT ON COLUMN tasks.performer_member_id IS 'Исполнитель';


--
-- TOC entry 2283 (class 0 OID 0)
-- Dependencies: 187
-- Name: COLUMN tasks.creator_member_id; Type: COMMENT; Schema: public; Owner: docflow
--

COMMENT ON COLUMN tasks.creator_member_id IS 'Создатель задачи';


--
-- TOC entry 2284 (class 0 OID 0)
-- Dependencies: 187
-- Name: COLUMN tasks.controller_member_id; Type: COMMENT; Schema: public; Owner: docflow
--

COMMENT ON COLUMN tasks.controller_member_id IS 'Контролирующий пользователь';


--
-- TOC entry 2285 (class 0 OID 0)
-- Dependencies: 187
-- Name: COLUMN tasks.status; Type: COMMENT; Schema: public; Owner: docflow
--

COMMENT ON COLUMN tasks.status IS 'Статус задачи';


--
-- TOC entry 2286 (class 0 OID 0)
-- Dependencies: 187
-- Name: COLUMN tasks.priority; Type: COMMENT; Schema: public; Owner: docflow
--

COMMENT ON COLUMN tasks.priority IS 'Приоритет задачи';


--
-- TOC entry 191 (class 1259 OID 25015)
-- Name: task_progress; Type: VIEW; Schema: public; Owner: docflow
--

CREATE VIEW task_progress AS
 SELECT tasks.id AS task_id,
        CASE
            WHEN (tasks.status = 'closed'::task_statuses) THEN (date_part('epoch'::text, age(tasks.end_timestamp, tasks.start_timestamp)))::integer
            ELSE (date_part('epoch'::text, age(now(), tasks.start_timestamp)))::integer
        END AS ticks_from_start,
    (date_part('epoch'::text, age(tasks.control_timestamp, tasks.start_timestamp)))::integer AS total_ticks
   FROM tasks;


ALTER TABLE task_progress OWNER TO docflow;

--
-- TOC entry 192 (class 1259 OID 25019)
-- Name: task_progress_in_percents; Type: VIEW; Schema: public; Owner: docflow
--

CREATE VIEW task_progress_in_percents AS
 SELECT task_progress.task_id,
        CASE
            WHEN (task_progress.total_ticks = 0) THEN 100
            WHEN (((task_progress.ticks_from_start / (task_progress.total_ticks / 100)) < 0) OR ((task_progress.ticks_from_start / (task_progress.total_ticks / 100)) > 100)) THEN 100
            ELSE (task_progress.ticks_from_start / (task_progress.total_ticks / 100))
        END AS percents_done,
        CASE
            WHEN (task_progress.total_ticks = 0) THEN true
            ELSE (((task_progress.ticks_from_start / (task_progress.total_ticks / 100)) < 0) OR ((task_progress.ticks_from_start / (task_progress.total_ticks / 100)) > 100))
        END AS overdue_flag
   FROM task_progress;


ALTER TABLE task_progress_in_percents OWNER TO docflow;

--
-- TOC entry 193 (class 1259 OID 25023)
-- Name: task_progress_statuses; Type: VIEW; Schema: public; Owner: docflow
--

CREATE VIEW task_progress_statuses AS
 SELECT task_progress_in_percents.task_id,
        CASE task_progress_in_percents.overdue_flag
            WHEN true THEN 'overdue'::text
            ELSE
            CASE
                WHEN ((task_progress_in_percents.percents_done >= 0) AND (task_progress_in_percents.percents_done < 70)) THEN 'normal'::text
                WHEN ((task_progress_in_percents.percents_done >= 70) AND (task_progress_in_percents.percents_done < 90)) THEN 'warning'::text
                WHEN ((task_progress_in_percents.percents_done >= 90) AND (task_progress_in_percents.percents_done <= 100)) THEN 'critical'::text
                ELSE NULL::text
            END
        END AS task_done_status,
        CASE task_progress_in_percents.overdue_flag
            WHEN true THEN 'Нарушен срок'::text
            ELSE
            CASE
                WHEN ((task_progress_in_percents.percents_done >= 0) AND (task_progress_in_percents.percents_done < 70)) THEN 'Время работ не достигло 70% от общего времени выполнения'::text
                WHEN ((task_progress_in_percents.percents_done >= 70) AND (task_progress_in_percents.percents_done < 90)) THEN 'Время работ между 70% и 90% от общего времени выполнения'::text
                WHEN ((task_progress_in_percents.percents_done >= 90) AND (task_progress_in_percents.percents_done <= 100)) THEN 'Время работ более 90% от общего времени выполнения'::text
                ELSE NULL::text
            END
        END AS task_done_description
   FROM task_progress_in_percents;


ALTER TABLE task_progress_statuses OWNER TO docflow;

--
-- TOC entry 190 (class 1259 OID 25011)
-- Name: task_timeranges; Type: VIEW; Schema: public; Owner: docflow
--

CREATE VIEW task_timeranges AS
 SELECT (now() > tasks.control_timestamp) AS overtime_control_time,
    to_char(age(tasks.control_timestamp, tasks.start_timestamp), 'YY|MM|DD|HH24|MI|SS'::text) AS total_task_from_start_to_control_time,
    to_char(age(now(), tasks.start_timestamp), 'YY|MM|DD|HH24|MI|SS'::text) AS left_task_from_start_to_now_time,
        CASE
            WHEN (now() > tasks.control_timestamp) THEN to_char((- age(tasks.control_timestamp, now())), 'YY|MM|DD|HH24|MI|SS'::text)
            ELSE to_char(age(tasks.control_timestamp, now()), 'YY|MM|DD|HH24|MI|SS'::text)
        END AS remaining_task_from_now_to_control_time,
        CASE
            WHEN (tasks.end_timestamp IS NULL) THEN NULL::text
            ELSE
            CASE
                WHEN (tasks.end_timestamp > tasks.control_timestamp) THEN to_char(age(tasks.end_timestamp, tasks.control_timestamp), 'YY|MM|DD|HH24|MI|SS'::text)
                ELSE to_char(age(tasks.control_timestamp, tasks.end_timestamp), 'YY|MM|DD|HH24|MI|SS'::text)
            END
        END AS total_task_from_control_to_end_time,
        CASE
            WHEN (tasks.end_timestamp IS NULL) THEN NULL::text
            ELSE to_char(age(tasks.end_timestamp, tasks.start_timestamp), 'YY|MM|DD|HH24|MI|SS'::text)
        END AS total_task_from_start_to_end_time,
    tasks.id AS task_id
   FROM tasks;


ALTER TABLE task_timeranges OWNER TO docflow;

--
-- TOC entry 186 (class 1259 OID 24849)
-- Name: tasks_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE tasks_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE tasks_id_seq OWNER TO docflow;

--
-- TOC entry 2287 (class 0 OID 0)
-- Dependencies: 186
-- Name: tasks_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE tasks_id_seq OWNED BY tasks.id;


--
-- TOC entry 195 (class 1259 OID 33135)
-- Name: uploaded_files; Type: TABLE; Schema: public; Owner: docflow
--

CREATE TABLE uploaded_files (
    id integer NOT NULL,
    mime character varying(128) NOT NULL,
    original_file_name character varying(512) NOT NULL,
    object_class objects_classes NOT NULL,
    class_id integer NOT NULL,
    uploaded_datetime timestamp with time zone DEFAULT now() NOT NULL,
    owner_id integer NOT NULL,
    file_name character varying(512) NOT NULL
);


ALTER TABLE uploaded_files OWNER TO docflow;

--
-- TOC entry 194 (class 1259 OID 33133)
-- Name: uploaded_files_id_seq; Type: SEQUENCE; Schema: public; Owner: docflow
--

CREATE SEQUENCE uploaded_files_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE uploaded_files_id_seq OWNER TO docflow;

--
-- TOC entry 2288 (class 0 OID 0)
-- Dependencies: 194
-- Name: uploaded_files_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: docflow
--

ALTER SEQUENCE uploaded_files_id_seq OWNED BY uploaded_files.id;


--
-- TOC entry 2104 (class 2604 OID 16658)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY changelog ALTER COLUMN id SET DEFAULT nextval('changelog_id_seq'::regclass);


--
-- TOC entry 2108 (class 2604 OID 16659)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY comments ALTER COLUMN id SET DEFAULT nextval('comments_id_seq'::regclass);


--
-- TOC entry 2095 (class 2604 OID 16660)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY divisions ALTER COLUMN id SET DEFAULT nextval('divisions_id_seq'::regclass);


--
-- TOC entry 2111 (class 2604 OID 16661)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY maintenance_work ALTER COLUMN id SET DEFAULT nextval('maintenance_work_id_seq'::regclass);


--
-- TOC entry 2109 (class 2604 OID 16662)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY maintenance_work_class ALTER COLUMN id SET DEFAULT nextval('maintenance_work_class_id_seq'::regclass);


--
-- TOC entry 2093 (class 2604 OID 16663)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY members ALTER COLUMN id SET DEFAULT nextval('members_id_seq'::regclass);


--
-- TOC entry 2101 (class 2604 OID 16664)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY tasks ALTER COLUMN id SET DEFAULT nextval('tasks_id_seq'::regclass);


--
-- TOC entry 2106 (class 2604 OID 16665)
-- Name: id; Type: DEFAULT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY uploaded_files ALTER COLUMN id SET DEFAULT nextval('uploaded_files_id_seq'::regclass);


--
-- TOC entry 2136 (class 2606 OID 16666)
-- Name: pk_changelog; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY changelog
    ADD CONSTRAINT pk_changelog PRIMARY KEY (id);


--
-- TOC entry 2142 (class 2606 OID 16667)
-- Name: pk_comments; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY comments
    ADD CONSTRAINT pk_comments PRIMARY KEY (id);


--
-- TOC entry 2116 (class 2606 OID 16668)
-- Name: pk_divisions; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY divisions
    ADD CONSTRAINT pk_divisions PRIMARY KEY (id);


--
-- TOC entry 2148 (class 2606 OID 16669)
-- Name: pk_maintenance_work; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY maintenance_work
    ADD CONSTRAINT pk_maintenance_work PRIMARY KEY (id);


--
-- TOC entry 2146 (class 2606 OID 16670)
-- Name: pk_maintenance_work_class; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY maintenance_work_class
    ADD CONSTRAINT pk_maintenance_work_class PRIMARY KEY (id);


--
-- TOC entry 2118 (class 2606 OID 16671)
-- Name: pk_member_rights; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY members_rights
    ADD CONSTRAINT pk_member_rights PRIMARY KEY (member_id, allow, object_class);


--
-- TOC entry 2113 (class 2606 OID 16672)
-- Name: pk_members; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY members
    ADD CONSTRAINT pk_members PRIMARY KEY (id);


--
-- TOC entry 2132 (class 2606 OID 16673)
-- Name: pk_tasks; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY tasks
    ADD CONSTRAINT pk_tasks PRIMARY KEY (id);


--
-- TOC entry 2144 (class 2606 OID 16674)
-- Name: pk_tasks_coworkers; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY task_coworkers
    ADD CONSTRAINT pk_tasks_coworkers PRIMARY KEY (task_id, coworker_member_id);


--
-- TOC entry 2140 (class 2606 OID 16675)
-- Name: pk_uploaded_files; Type: CONSTRAINT; Schema: public; Owner: docflow
--

ALTER TABLE ONLY uploaded_files
    ADD CONSTRAINT pk_uploaded_files PRIMARY KEY (id);


--
-- TOC entry 2133 (class 1259 OID 24998)
-- Name: i_changelog_changer_member_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_changelog_changer_member_id ON changelog USING btree (changer_member_id);


--
-- TOC entry 2134 (class 1259 OID 24999)
-- Name: i_changelog_class_and_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_changelog_class_and_id ON changelog USING btree (class_of_change, class_id);


--
-- TOC entry 2114 (class 1259 OID 24931)
-- Name: i_divisions_parent_division_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_divisions_parent_division_id ON divisions USING btree (parent_division);


--
-- TOC entry 2119 (class 1259 OID 33690)
-- Name: i_tasks_control_timestamp; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_control_timestamp ON tasks USING btree (control_timestamp);


--
-- TOC entry 2120 (class 1259 OID 25030)
-- Name: i_tasks_controller_member_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_controller_member_id ON tasks USING btree (controller_member_id);


--
-- TOC entry 2121 (class 1259 OID 25029)
-- Name: i_tasks_cperformer_member_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_cperformer_member_id ON tasks USING btree (performer_member_id);


--
-- TOC entry 2122 (class 1259 OID 25028)
-- Name: i_tasks_creator_member_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_creator_member_id ON tasks USING btree (creator_member_id);


--
-- TOC entry 2123 (class 1259 OID 33691)
-- Name: i_tasks_end_timestamp; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_end_timestamp ON tasks USING btree (end_timestamp);


--
-- TOC entry 2124 (class 1259 OID 24951)
-- Name: i_tasks_members; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_members ON tasks USING btree (performer_member_id, creator_member_id, controller_member_id);


--
-- TOC entry 2125 (class 1259 OID 33173)
-- Name: i_tasks_members_division_status; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_members_division_status ON tasks USING btree (target_division_id, performer_member_id, creator_member_id, controller_member_id, status);


--
-- TOC entry 2126 (class 1259 OID 25033)
-- Name: i_tasks_priority; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_priority ON tasks USING btree (priority);


--
-- TOC entry 2127 (class 1259 OID 33689)
-- Name: i_tasks_start_timestamp; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_start_timestamp ON tasks USING btree (start_timestamp);


--
-- TOC entry 2128 (class 1259 OID 25032)
-- Name: i_tasks_status; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_status ON tasks USING btree (status);


--
-- TOC entry 2129 (class 1259 OID 25031)
-- Name: i_tasks_target_division_id; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_target_division_id ON tasks USING btree (target_division_id);


--
-- TOC entry 2130 (class 1259 OID 33692)
-- Name: i_tasks_timestamps; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_tasks_timestamps ON tasks USING btree (start_timestamp, control_timestamp, end_timestamp);


--
-- TOC entry 2137 (class 1259 OID 33147)
-- Name: i_uploaded_files_class; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_uploaded_files_class ON uploaded_files USING btree (object_class, class_id);


--
-- TOC entry 2138 (class 1259 OID 33148)
-- Name: i_uploaded_files_owner; Type: INDEX; Schema: public; Owner: docflow
--

CREATE INDEX i_uploaded_files_owner ON uploaded_files USING btree (owner_id);


--
-- TOC entry 2273 (class 0 OID 0)
-- Dependencies: 7
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- Completed on 2017-03-14 10:02:52 MSK

--
-- PostgreSQL database dump complete
--

