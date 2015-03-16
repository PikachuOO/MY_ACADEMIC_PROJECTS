-- COMP3311 13s2 Assignment 2
-- Written by Jiashu Chen, September 2013
-- Q1: ...
create or replace view Q1(unswid, name)
as
    select p.unswid, p.name
    from Course_enrolments c
        join People p on (c.student = p.id)
    group by p.unswid, p.name
    having count(*) > 65
;
-- Q2: ...
create or replace view Q2(nstudents, nstaff, nboth)
as
    select
        (select count(stu_only)
            from (
                (select id from Students) except
                (select id from Staff)
            ) as stu_only
        ),
        (select count(sta_only)
            from (
                (select id from Staff) except
                (select id from Students)
            ) as sta_only
        ),
        (select count(both_both)
            from (
                (select id from Staff) intersect
                (select id from Students)
            ) as both_both
        )
;
-- Q3: ...
-- get staff.id | counts
create or replace view Q3_helper(staff, ncourses)
as
    select staff, count(*)
    from Course_staff
    where role in (
        select id
        from Staff_roles
        where name like 'Course Convenor'
        )
    group by staff
;
create or replace view Q3(name, ncourses)
as
    select p.name, h.ncourses
    from Q3_helper h
        left outer join People p on (p.id = h.staff)
    where h.ncourses = (
        select max(ncourses)
        from Q3_helper)
;
-- Q4: ...
create or replace view sem_05s2(id)
as
    select id
    from semesters
    where year = 2005 and term like 'S2'
;
create or replace view program_compSci(id)
as
    select id
    from programs
    where code ilike '3978';
;
create or replace view Q4a_helper(id)
as
    select student
    from Program_enrolments
    where semester in (
        select * from sem_05s2
        )
    and program in (
        select * from program_compSci
        )
;
create or replace view Q4a(id)
as
    select unswid
    from People
    where people.id in (select * from Q4a_helper)
;
-- helper for Q4b
create or replace view stream_sengA1(id)
as
    select id
    from streams
    where code ilike 'SENGA1';
;
create or replace view Q4b_helper(id)
as
    select p.student
    from Program_enrolments p
        join Stream_enrolments s on (s.partof = p.id)
    where semester in (
        select * from sem_05s2
        )
    and s.stream in (
        select * from stream_sengA1
        )
;
create or replace view Q4b(id)
as
    select unswid
    from People
    where people.id in (select * from Q4b_helper)
;
-- helper for Q4c
create or replace view degree_cse(id)
as
    select id
    from Programs
    where offeredby in (select id
                        from orgunits
                        where longname ilike '%Computer Science and Engineering');
;
create or replace view Q4c_helper(id)
as
    select student
    from Program_enrolments
    where semester in (
        select * from sem_05s2
        )
    and program in (
        select * from degree_cse
        )
;
create or replace view Q4c(id)
as
    select unswid
    from People
    where people.id in (select * from Q4c_helper)
;
-- Q5: ...
create or replace view Q5_helper(id, counter)
as
    select facultyOf(id), count(*)
    from OrgUnits
    where name ilike '%committee' and facultyOf(id) is not null
    group by facultyOf(id)
;
create or replace view Q5(name)
as
    select o.name
    from Q5_helper h
        left outer join OrgUnits o on (o.id = h.id)
    where h.counter = (select max(counter) from Q5_helper)
;
-- Q6: ...
create or replace function Q6(integer) returns text
as
$$
    select name
    from people
    where id = $1 or unswid = $1
$$ language sql
;
-- Q7: ...
create or replace function Q7(text)
    returns table (course text, year integer, term text, convenor text)
as $$
    select
            cast(sub.code as text),
            sem.year as year,
            cast(sem.term as text),
            cast(pe.name as text)
    from courses c
        join Course_staff s on (s.course = c.id)
        join subjects sub on (sub.id = c.subject)
        join semesters sem on (sem.id = c.semester)
        join staff sta on (sta.id = s.staff)
        join people pe on (pe.id = sta.id)
    where sub.code ilike $1
        and s.role in (
            select id
            from Staff_roles
            where name like 'Course Convenor'
            )
$$ language sql
;
-- Q8: ...
create or replace function Q8_prog(integer, text)
    returns char(4)
as $$
    select pro.code
        from courses c
        join Course_enrolments ce on (ce.course = c.id)
        join Semesters s on (s.id = c.semester)
        join people p on (p.id = ce.student)
        join Program_enrolments pe on (pe.student = p.id and s.id = pe.semester)
        join programs pro on (pro.id = pe.program)
        join subjects sub on (sub.id = c.subject)
        where p.unswid = $1 and sub.code = $2;
$$ language sql
;
create or replace function Q8(integer)
    returns setof NewTranscriptRecord
as $$
declare
    _inputID alias for $1;
    ret_val NewTranscriptRecord%rowtype;
    iter record;
begin
    for iter in select * from transcript(_inputID)
    loop
        ret_val.code = iter.code;
        ret_val.term = iter.term;
        ret_val.name = iter.name;
        ret_val.mark = iter.mark;
        ret_val.grade = iter.grade;
        ret_val.uoc = iter.uoc;
        ret_val.prog := Q8_prog(_inputID, iter.code);
        return next ret_val;
    end loop;
    return;
end;
$$ language plpgsql
;
-- Q9: ...
create or replace function Q9_Object_code(text)
        returns table (course char(4))
as $$
    select code
    from subjects
    where code similar to $1
$$ language sql
;
create or replace function Q9(integer)
    returns setof AcObjRecord
as $$
declare
    query_string text;
    definition text;
    aca_obj_group record;
    _inputID alias for $1;
    iter text;
    course text;
    ac_obj_record AcObjRecord%rowtype;
begin
    query_string := 'select * from acad_object_groups where id = '|| _inputID;
    execute query_string into aca_obj_group;
    if (aca_obj_group.gdefby not ilike 'pattern'
            or aca_obj_group.gtype not ilike 'subject') then
        -- raise EXCEPTION 'Invalid acad_object_groups.gdefby type ''%''', aca_obj_group.gdefby;
        return;
    end if;
    if (aca_obj_group.definition ilike '%[\w]*/[A-Z]=[A-Z]+%'
        or aca_obj_group.definition similar to '%{\w+?;\w+?}%'
        or aca_obj_group.definition similar to '%(FREE|GENG)%') then
        -- raise EXCEPTION 'Invalid acad_object_groups.definition method ''%''', aca_obj_group.definition;
        return;
    end if;
    definition = aca_obj_group.definition;
    -- -- deal with /F=ENG
    -- definition = regexp_replace(definition, ',?[\w]*/[A-Z]=[A-Z]+,?', ',', 'g');
    -- deal with ####3###
    definition = replace (replace(definition, '#', '\w'), ' ', '');
    -- deal with {COMP2041; COMP1911}
    -- definition = replace(replace(replace(definition, '{', '('), '}', ')'), ';', '|');
    -- -- select * from regexp_split_to_table(definition, ',')
    for iter in select * from regexp_split_to_table(definition, ',')
    loop
        for course in select * from Q9_Object_code(iter)
        loop
            ac_obj_record.objtype = aca_obj_group.gtype;
            ac_obj_record.object = course;
            return next ac_obj_record;
        end loop;
    end loop;
end;
$$ language plpgsql
;