-- COMP3311 12s1 Assignment 3
--
-- updates.sql: changes to supplied database
-- 
-- Written by <<Jiashu Chen>>, October 2013
--
-- Must include all of the SQL/PLpgSQL infrastructure that you
--   have added to the supplied database to support your PHP code
-- This must be complete (i.e. contain all required additions)
--   and must load in a single pass (i.e. no forward references)
--   and must be syntax-error free (i.e. tested before submission)

-- function select course matching regex_pattern

create or replace function TaskA_get_all_group(integer)
        returns table (ao_group integer)
as $$
    select id 
    from acad_object_groups 
    where id = $1 or parent = $1;
$$ language sql
;

create or replace function TaskA_enumerated(text, integer)
    returns setof text
as $$
declare
    query_string text;
    _group_type alias for $1;
    _input_ID alias for $2;
    courses_selected record;
    ret_val text;
begin
    query_string := 'select ret.code
                    from '|| _group_type||'_group_members gm_table '||'
                        join '||_group_type||'s ret on (gm_table.'||_group_type||' = ret.id)
                    where gm_table.ao_group in 
                        (select * from TaskA_get_all_group('||_input_ID||'));';
    for courses_selected in execute query_string
    loop
        ret_val = courses_selected.code;
        return next ret_val;
    end loop;
    return;
end;
$$ language plpgsql
;

create or replace function TaskA_regex_matcher(text, text)
    returns setof text
as $$
declare
    _group_type alias for $1;
    _pattern alias for $2;
    query_string text;
    sub_pattern text;
    definition text;
    course record;
begin
    definition := _pattern;
    -- deal with ####3###
    definition = replace (replace(definition, '#', '\w'), ' ', '');
    -- deal with {COMP2041; COMP1911}
    definition = replace(replace(replace(definition, '{', '('), '}', ')'), ';', '|');
    for sub_pattern in select * from regexp_split_to_table(definition, ',')
    loop
        query_string := 'select code
                    from '||_group_type||'s
                    where code similar to '''||sub_pattern||'''';
    -- deal with /F=ENG and since this only happen when
    -- gtype is subject so I can do this as 
    -- if it happens to all cases
        -- raise notice 'current query is ''%''', query_string;

        if (sub_pattern similar to '%(FREE|GENG)%'
            or sub_pattern similar to '%\\w\\w\\w\\w'
            or sub_pattern similar to '\\w\\w\\w\\w%') then
            sub_pattern = replace(sub_pattern, '\w', '#');
            -- raise notice 'current sub_pattern is ''%''', sub_pattern;
            return next sub_pattern;
        else 
            if (sub_pattern similar to '%/[A-Z]=[!A-Z]+%') then
                sub_pattern = regexp_replace(sub_pattern, '/[A-Z]=', ''' and orgunits.unswid similar to ''');
                query_string := 'select s.code
                        from '||_group_type||'s s
                            join orgunits on (orgunits.id = s.offeredby)
                        where s.code similar to '''||sub_pattern||'''';
            end if;
            for course in execute query_string
            loop
                return next course.code;
            end loop;
        end if;

    end loop;
end;
$$ language plpgsql
;


create or replace function TaskB_regex_matcher(text, text)
    returns setof text
as $$
declare
    _group_type alias for $1;
    _pattern alias for $2;
    query_string text;
    sub_pattern text;
    definition text;
    course record;
begin
    definition := _pattern;
    -- deal with ####3###
    definition = regexp_replace(definition, '^###', '[^G][^E][^N]');
    definition = replace (replace(definition, '#', '\w'), ' ', '');
    -- deal with {COMP2041; COMP1911}
    definition = replace(replace(replace(definition, '{', '('), '}', ')'), ';', '|');
    -- deal with FREE and GENG
    definition = replace(definition, 'FREE', '\w\w\w\w');
    definition = replace(definition, 'GENG', 'GEN\w');
    for sub_pattern in select * from regexp_split_to_table(definition, ',')
    loop
        query_string := 'select code
                    from '||_group_type||'s
                    where code similar to '''||sub_pattern||'''';
        -- deal with /F=ENG and since this only happen when
        -- gtype is subject so I can do this as 
        -- if it happens to all cases

        if (sub_pattern similar to '%/[A-Z]=[!A-Z]+%') then
            sub_pattern = regexp_replace(sub_pattern, '/[A-Z]=', ''' and orgunits.unswid similar to ''');
            query_string := 'select s.code
                    from '||_group_type||'s s
                        join orgunits on (orgunits.id = s.offeredby)
                    where s.code similar to '''||sub_pattern||'''';
        raise notice 'current query_string is ''%''', query_string;
        end if;
        -- raise notice 'current query is ''%''', query_string;

        for course in execute query_string
        loop
            return next course.code;
        end loop;
    end loop;
end;
$$ language plpgsql
;

create or replace function TaskD_student_program(integer, integer)
-- function take in two variable people.id and semester.id
        returns table (id integer, program integer)
as $$
    select id,program 
    from program_enrolments
    where student = $1 and semester = $2;
$$ language sql
;

create or replace function TaskD_student_stream(integer)
-- function take in one variable program_enrolments.id
        returns integer
as $$
    select stream 
    from stream_enrolments
    where partof = $1;
$$ language sql
;


create or replace function TaskD_get_all_rules_id(integer, integer)
-- function takein program.id and stream.id
    returns table (ruleID integer)
as $$
    (select rule
    from program_rules
    where program = $1)
    union
    (select rule
    from stream_rules
    where stream = $2)
$$ language sql
;

create or replace function TaskD_get_all_rules_id(integer, integer)
-- function takein program.id and stream.id
    returns table (ruleID integer)
as $$
    (select rule
    from program_rules
    where program = $1)
    union
    (select rule
    from stream_rules
    where stream = $2)
$$ language sql
;

create or replace function TaskE_get_career(integer)
        returns text
as $$
    select career::text
    from programs 
    where id = $1;
$$ language sql
;
