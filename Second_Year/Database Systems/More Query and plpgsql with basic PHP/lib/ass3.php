<?php 
// COMP3311 13s2 Assignment 3
// Functions for assignment Tasks A-E
// Written by <<Jiashu Chen>>, October 2013

// assumes that defs.php has already been included


// Task A: get members of an academic object group

// E.g. list($type,$codes) = membersOf($db, 111899)
// Inputs:
//  $db = open database handle
//  $groupID = acad_object_group.id value
// Outputs:
//  array(GroupType,array(Codes...))
//  GroupType = "subject"|"stream"|"program"
//  Codes = acad object codes in alphabetical order
//  e.g. array("subject",array("COMP2041","COMP2911"))

function membersOf($db,$groupID)
{
    $q = "SELECT * FROM acad_object_groups WHERE id = %d";
    $grp = dbOneTuple($db, mkSQL($q, $groupID));
    // Get a handle on some column values.
    $gDefby = $grp['gdefby'];
    $gType = $grp["gtype"];
    $definition = $grp["definition"];
    
    if ($gDefby == "enumerated") {
        $q = "SELECT TaskA_enumerated as code FROM TaskA_enumerated('".$gType."', %d)";
        $tempArray = dbAllTuples($db, mkSQL($q, $groupID));
        $resultArray = array();
        foreach($tempArray as $row) {
            array_push($resultArray, $row["code"]);
        }
        return array($grp["gtype"], $resultArray);
    }elseif ($gDefby == "pattern") {
        $pattern = $definition;
        if ($pattern) {
            $q = "SELECT TaskA_regex_matcher as code 
                FROM TaskA_regex_matcher('".$gType."', %s)
                order by code ASC";
            $tempArray = dbAllTuples($db, mkSQL($q, $pattern));
            $resultArray = array();
            foreach($tempArray as $row) {
                array_push($resultArray, $row["code"]);
            }
            return array($grp["gtype"], $resultArray);
        }
    }elseif ($gDefby = "querys") {
        $querys = $definition;
        $tempArray = dbAllTuples($db, mkSQL($q));
        foreach($tempArray as $row) {
            array_push($resultArray, $row["code"]);
        }
        return array($grp["gtype"], $resultArray);
    }
    return array($grp["gtype"], array("########"));
}


// Task B: check if given object is in a group

// E.g. if (inGroup($db, "COMP3311", 111938)) ...
// Inputs:
//  $db = open database handle
//  $code = code for acad object (program,stream,subject)
//  $groupID = acad_object_group.id value
// Outputs:
//  true/false

function inGroup($db, $code, $groupID)
{

    $q = "SELECT * FROM acad_object_groups WHERE id = %d";
    $grp = dbOneTuple($db, mkSQL($q, $groupID));
    // Get a handle on some column values.
    $gDefby = $grp['gdefby'];
    $gType = $grp["gtype"];
    $definition = $grp["definition"];
    if ($gDefby == "enumerated") {
        $q = "SELECT TaskA_enumerated as code FROM TaskA_enumerated('".$gType."', %d)";
        $tempArray = dbAllTuples($db, mkSQL($q, $groupID));
        $resultArray = array();
        foreach($tempArray as $row) {
            array_push($resultArray, $row["code"]);
        }
    }elseif ($gDefby == "pattern") {
        $pattern = $definition;
        if ($pattern) {
            $q = "SELECT TaskB_regex_matcher as code 
                FROM TaskB_regex_matcher('".$gType."', %s)
                order by code ASC";
            $tempArray = dbAllTuples($db, mkSQL($q, $pattern));
            $resultArray = array();
            foreach($tempArray as $row) {
                array_push($resultArray, $row["code"]);
            }
            }
    }elseif ($gDefby = "querys") {
        $querys = $definition;
        $tempArray = dbAllTuples($db, mkSQL($q));
        foreach($tempArray as $row) {
            array_push($resultArray, $row["code"]);
        }
    }
    // test
    // foreach ($resultArray as $code) echo "$code\n";

    return in_array($code, $resultArray);
}


// Task C: can a subject be used to satisfy a rule

// E.g. if (canSatisfy($db, "COMP3311", 2449, $enr)) ...
// Inputs:
//  $db = open database handle
//  $code = code for acad object (program,stream,subject)
//  $ruleID = rules.id value
//  $enr = array(ProgramID,array(StreamIDs...))
// Outputs:

function canSatisfy($db, $code, $ruleID, $enrolment)
{

    $ret_val = false;
    $q = "SELECT ao_group, type FROM rules 
            WHERE id = %d and type not in ('LR','MR','WM','IR')";
    $tempArray = dbOneTuple($db, mkSQL($q,$ruleID));
    if (sizeof($tempArray > 0)) {
        // echo $tempArray["ao_group"];
        if (inGroup($db,$code,$tempArray["ao_group"])) {
            $ret_val = true;
            if (preg_match("/^GEN/", $code)) {
                // stream exists
                $faculty_array = array();
                if(sizeof($enrolment[1]) > 0) {                  
                    $check_string = implode(", ", $enrolment[1]);
                    $q = "SELECT DISTINCT facultyof(offeredby) 
                            FROM streams WHERE id IN (%s)";
                    $tempArray = dbAllTuples($db, mkSQL($q, $check_string));
                    foreach($tempArray as $row) {
                        array_push($faculty_array, $row["facultyof"]);
                    }
                }else {
                    $q = "SELECT DISTINCT facultyof(offeredby) 
                            FROM programs WHERE id = (%d)";
                    $tempArray = dbOneTuple($db, mkSQL($q,$enrolment[0]));
                    array_push($faculty_array, $tempArray["facultyof"]);
                }
                $q = "SELECT facultyof(offeredby) 
                        FROM subjects where code ilike %s";
                $tempArray = dbOneTuple($db, mkSQL($q,$code));
                if (in_array($tempArray["facultyof"], $faculty_array)) {
                    $ret_val = false;
                }
            }                
        }
    }
    return $ret_val;
}


// Task D: determine student progress through a degree

// E.g. $vtrans = progress($db, 3012345, "05s1");
// Inputs:
//  $db = open database handle
//  $stuID = People.id value (i.e. unsw student id)
//  $semester = code for semester (e.g. "09s2")
// Outputs:
//  Virtual transcript array (see spec for details)


function check_semester($db, $stuID, $term)
{
    $all_semester = array();
    $all_enroled_semster_query = "SELECT semester 
                from program_enrolments 
                where student = %d";
    $tempArray = dbAllTuples($db, mkSQL($all_enroled_semster_query,$stuID));
    foreach($tempArray as $row) {
        array_push($all_semester, $row["semester"]);
    }
    if (in_array($term, $all_semester)) {
        return $term;
    } else {
        $check_string = implode(", ", $all_semester);
        $q = "SELECT id from semesters 
            where id in ($check_string) order by starting desc";
        $temp = dbOneTuple($db, mkSQL($q));
        return $temp["id"];
    }
}

function get_all_rules($db, $stuID, $term)
{
    $all_rules = array();

    $program_query = "SELECT * FROM taskd_student_program(%d,%d)";
    $stream_query = "SELECT * FROM taskd_student_stream(%d)";
    $rule_query = "SELECT * FROM TaskD_get_all_rules_id(%d,%d)";

    $temp = dbOneTuple($db, mkSQL($program_query,$stuID,$term));
    $program = $temp["program"];
    $partof = $temp["id"];
    $temp = dbAllTuples($db, mkSQL($stream_query,$partof));
    $stream_array = array();
    foreach ($temp as $row) {
        $stream = $row["taskd_student_stream"];
        array_push($stream_array, $stream);
        // echo $program," ", $stream,"\n";
        $tempArray = dbAllTuples($db, mkSQL($rule_query,$program,$stream));
        foreach($tempArray as $row) {
            array_push($all_rules, $row["ruleid"]);
        }

    }

    $valid_rule_query = "SELECT id, min, name,ao_group FROM rules 
                        WHERE id IN (".implode(", ", $all_rules).")
                        AND type not in ('DS', 'IR', 'RC', 'MR', 'RQ', 'WM')
                        ORDER BY CASE 
                        WHEN type='CC' THEN 0 
                        WHEN type='PE' THEN 1 
                        WHEN type='FE' THEN 2 
                        WHEN type='GE' THEN 3 
                        WHEN type='LR' THEN 4 
                        ELSE 6 END, id ASC";

    $tempArray = dbAllTuples($db, mkSQL($valid_rule_query));
    $valided_rules = array();
    foreach($tempArray as $row) {
        array_push($valided_rules, array(
                'id' => $row["id"],
                'uoc_complete' => 0,
                'required_uoc' => $row["min"],
                'ao_group' => $row["ao_group"],
                'name' => $row["name"],
                // help debuging
                'readable' => showRule($db, $row["id"])
                ));
    }

    return array($program, $stream_array, $all_rules, $valided_rules);
}

function check_requirements($db,&$valided_rules,$enrolment,$code,$term,$name,$mark,$grade,$uoc)
{

    // pre-checking case, doesn't require rule checking
    if ($mark == "" && $grade == "") {
        return "Incomplete. Does not yet count";  
    } elseif (preg_match("/FL/", $grade)) {
        return "Failed. Does not count";
    }

    $requirement = '';
    $valid = false;
    $fitted_rule = "";

    foreach ($valided_rules as &$rule) {
        // test if a rule is worth checking
        if ($rule["uoc_complete"] < $rule["required_uoc"]
            && preg_match("/HD|DN|CR|SY|PC|PS|PT/", $grade)) {
            if (canSatisfy($db,$code,$rule["id"],$enrolment)) {
                $rule["uoc_complete"] += $uoc;
                $valid = true;
                $fitted_rule = $rule;
                // print_r($fitted_rule);
                // echo "$code fitted rule",$rule["readable"],"\n";
                break;
            }
        }
    }

    // determining the return value base on rule selected
    if (preg_match("/HD|DN|CR|SY|PC|PS|PT/", $grade)
        && $valid) {
        $requirement = $fitted_rule["name"];
    } elseif (preg_match("/HD|DN|CR|SY|PC|PS|PT/", $grade)
        && !$valid) {
        $requirement = "Fits no requirement. Does not count";
    } else {
        $requirement = "Outside of My Script Scope";
    }
    return $requirement;
}

function progress($db, $stuID, $term)
{
    $ret_val = array();
    $input_term = $term;
    // echo $stuID," ", $term,"\n";
    $term = check_semester($db, $stuID, $term);
    // echo $stuID," ", $term,"\n";
    $all_rules = get_all_rules($db, $stuID, $term);
    $valided_rules = $all_rules[3];
    $all_rules_id = $all_rules[2];
    $enrolment = array($all_rules[0], $all_rules[1]);
    // print_r($enrolment);
    $q = "SELECT * 
            FROM transcript(%d, %d)";
    $transcript = dbAllTuples($db, mkSQL($q, $stuID, $input_term));
    // print_r($transcript);
    foreach ($transcript as $row) {
        if($row["code"] == null && $row["term"] == null && $row["grade"] == null) {
            $progress_element = array("Overall WAM", $row["mark"], $row["uoc"]);
        } else {
            $requirement = check_requirements($db,
                                        $valided_rules,
                                        $enrolment,
                                        $row["code"], 
                                        $row["term"], 
                                        $row["name"], 
                                        $row["mark"], 
                                        $row["grade"], 
                                        $row["uoc"]);
            if (preg_match('/^Failed/', $requirement)) {
                $uoc = '';
            } elseif (preg_match('/^Incomplete/', $requirement)){
                $uoc = null;
            }else {
                $uoc = $row["uoc"];
            }
            $progress_element = array($row["code"],
                                        $row["term"],
                                        $row["name"],
                                        $row["mark"],
                                        $row["grade"],
                                        $uoc,
                                        $requirement);
        }
        array_push($ret_val, $progress_element);
    }

    foreach($valided_rules as $rule){
        if(($rule["required_uoc"] > $rule["uoc_complete"]))
        {
         array_push($ret_val, array($rule["uoc_complete"]." UOC so far; need "
                                       .($rule["required_uoc"]-$rule["uoc_complete"])." UOC more", $rule["name"]));
        }
    }
    return $ret_val;
}


// Task E:

// E.g. $advice = advice($db, 3012345, 162, 164)
// Inputs:
//  $db = open database handle
//  $studentID = People.id value (i.e. unsw student id)
//  $currTermID = code for current semester (e.g. "09s2")
//  $nextTermID = code for next semester (e.g. "10s1")
// Outputs:
//  Advice array (see spec for details)


function TaskE_progress($db, $stuID, $term)
{
    $ret_val = array();
    $input_term = $term;
    // echo $stuID," ", $term,"\n";
    $term = check_semester($db, $stuID, $term);
    echo $stuID," ", $term,"\n";
    $all_rules = get_all_rules($db, $stuID, $term);
    $valided_rules = $all_rules[3];
    $all_rules_id = $all_rules[2];
    $enrolment = array($all_rules[0], $all_rules[1]);
    // print_r($enrolment);
    $q = "SELECT * 
            FROM transcript(%d, %d)";
    $transcript = dbAllTuples($db, mkSQL($q, $stuID, $input_term));
    // print_r($transcript);
    foreach ($transcript as $row) {
        if($row["code"] == null && $row["term"] == null && $row["grade"] == null) {
            $progress_element = array("Overall WAM", $row["mark"], $row["uoc"]);
        } else {
            $requirement = check_requirements($db,
                                        $valided_rules,
                                        $enrolment,
                                        $row["code"], 
                                        $row["term"], 
                                        $row["name"], 
                                        $row["mark"], 
                                        $row["grade"], 
                                        $row["uoc"]);
            if (preg_match('/^Failed/', $requirement)) {
                $uoc = '';
            } elseif (preg_match('/^Incomplete/', $requirement)){
                $uoc = null;
            }else {
                $uoc = $row["uoc"];
            }
            $progress_element = array($row["code"],
                                        $row["term"],
                                        $row["name"],
                                        $row["mark"],
                                        $row["grade"],
                                        $uoc,
                                        $requirement);
        }
        array_push($ret_val, $progress_element);
    }
    $uncomplete_Reqs = array();
    foreach($valided_rules as $rule){
        if(($rule["required_uoc"] > $rule["uoc_complete"]))
        {
            array_push($ret_val, array($rule["uoc_complete"]." UOC so far; need "
                                       .($rule["required_uoc"]-$rule["uoc_complete"])." UOC more", $rule["name"]));
            array_push($uncomplete_Reqs, $rule);
        }
    }
    return array($ret_val, $uncomplete_Reqs);
}

function advice($db, $studentID, $currTermID, $nextTermID)
{
    $ret_val = array();
    $currTermID = check_semester($db,$studentID,$currTermID);
    $nextTermID = check_semester($db,$studentID,$nextTermID);
    // $output = implode("", array("Student ID: ".$studentID."\n", 
    //         "Current Term: ".$currTermID."\n",
    //         "Next term: ".$nextTermID."\n"));
    // // echo $output,"\n";
    $all_rules = get_all_rules($db, $studentID, $currTermID);
    $enrolment = array($all_rules[0], $all_rules[1]);
    // print_r($enrolment);
    $q = "SELECT * from TaskE_get_career(%d)";
    $career = dbOneTuple($db, mkSQL($q, $enrolment[0]));
    $career = $career['taske_get_career'];
    // echo $career;

    list($currentProgress, $uncomplete_Reqs) = TaskE_progress($db, $studentID, $currTermID);
    $completed_course = array();
    $completed_course_query = "SELECT distinct code From transcript(%d,%d)
                                where name not like 'Overall WAM'";
    $tempArray = dbAllTuples($db, mkSQL($completed_course_query,$studentID,$nextTermID));
    foreach($tempArray as $row) {
        array_push($completed_course, $row["code"]);
    }
    // print_r($uncomplete_Reqs);
    $suggested_course = array();
    foreach ($uncomplete_Reqs as $key => $rule) {
        if (preg_match("/(\d+) UOC .* Free Electives/", $rule['readable'], $check)) {
            $push = array("Free....","Free Electives (many choices)",$rule['required_uoc']-$rule['uoc_complete'],"Free Electives");
            // print_r($push);
            array_push($ret_val,$push);
        }elseif (preg_match("/(\d+) UOC.*General Education courses/", $rule['readable'], $check)) {
            $push = array("GenEd...","General Education (many choices)",$rule['required_uoc']-$rule['uoc_complete'],"Gen Ed");
            // print_r($push);
            array_push($ret_val,$push);
        } elseif (preg_match("/(\d+) UOC from ([A-Za-z0-9\,\[\]\#]+)/", $rule['readable'], $check)) {
            list($type,$members) = membersOf($db, $rule['ao_group']);
            foreach ($members as $key => $code) $members[$key] = "'".$code."'";
            $career_subject = implode(",", $members);
            $q = "select code,uoc,name from subjects where code in ($career_subject) and career = %s order by code";
            echo mkSQL($q,$career), "\n";
            $temp_subjects = dbAllTuples($db, mkSQL($q,$career));
            $potential_subjects = array();
            $course_uoc = array();
            foreach ($temp_subjects as $key) {
                array_push($potential_subjects, $key["code"]);
                $course_uoc[$key["code"]] = $key["uoc"];
                $course_name[$key["code"]] = $key["name"];
            }
            foreach ($potential_subjects as $key => $code) {
                if (!in_array($code, $completed_course)
                    && !in_array($code, $suggested_course)
                    && ($rule['required_uoc']-$rule['uoc_complete']) >= $course_uoc[$code]
                    && $course_uoc[$code] != 0) {
                    array_push($suggested_course, $code);
                    $push = array($code,$course_name[$code],$course_uoc[$code],$rule["name"]);
                    array_unshift($ret_val, $push);
                }
            }
        }
    }
    return $ret_val;
}
?>
