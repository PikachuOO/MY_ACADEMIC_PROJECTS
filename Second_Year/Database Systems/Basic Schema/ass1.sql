-- COMP3311 13s2 Assignment 1
-- Schema for the mypics.net photo-sharing site
--
-- Written by <<Jiashu Chen>>
--
-- Conventions:
-- * all entity table names are plural
-- * most entities have an artifical primary key called "id"
-- * foreign keys are named after the relationship they represent

-- Domains (you may add more)

create domain URLValue as
	varchar(100) check (value like 'http://%');

create domain EmailValue as
	varchar(100) check (value like '%@%.%');

create domain GenderValue as
	varchar(6) check (value in ('male','female'));

create domain GroupModeValue as
	varchar(15) check (value in ('private','by-invitation','by-request'));

create domain ContactListTypeValue as
	varchar(10) check (value in ('friends','family'));

create domain NameValue as varchar(50);

create domain LongNameValue as varchar(100);

create domain Visibility_TypeValue as
	varchar(15) check (value in ('private','friends','friends+family','public'));

create domain Safety_Level_TypeLevel as
	varchar(10) check (value in ('safe','moderate','restricted'));

-- Tables (you must add more)

create table People (
	id 					serial,
	family_name			NameValue,
	give_name			NameValue,
	displayed_name		LongNameValue
	email_address		EmailValue,
	primary key (id)
);

create table Users (
	id 					integer constraint Valid_People,
	website				URLValue,
	date_registered		date,
	gender 				GenderValue,
	birthday			date,
	password			text,
	has_portorait		integer,
	primary key (id),
	foreign key id references People(id),
	foreign key has_portrait references Photos(id)
);

create table Groups (
	id 				serial,
	title			text not null,
	mode 			GroupModeValue,
	owned_by		integer,
	primary key (id),
	foreign key owned_by references Users(id)
);

create table Groups_member_Users (
	user_member 	integer,
	group_id		integer.
	foreign key user_member references Users(id),
	foreign key group_id references Groups(id),
	primary key (user_member, group_id)
);

create table Contact_lists (
	id				serial,
	type 			ContactListTypeValue,
	title			text not null,
	owned_by		integer,
	primary key (id),
	foreign key owned_by references Users(id)
);

create table Contact_lists_member_People (
	user_contact 	integer,
	contact_list 	integer,
	foreign key user_contact references Users(id),
	foreign key Contact_list references Contact_lists(id),
	primary key (user_contact, contact_list)
);

create table Discussions (
   id 			serial,
   title 		NameValue, 
   primary key (id)
);

create table Photos (
	id					serial,
	date_Taken			date not null,
	title				varchar(50) not null,
	date_uploaded		date not null,
	description 		text,
	file_size			integer,
	visibility			Visibility_TypeValue,
	safety_level		Safety_Level_TypeLevel,
	technical_details 	text,
	owned_by 			integer not null,
	had_in				integer,
	foreign key owned_by references Users(id),
	foreign key had_in references Discussions(id),
	primary key (id)
);

create table Users_rates_Photos (
	user_id 		integer,
	photo_id		integer,
	when_rated		timestamp not null,
	rating			integer constraint Valid_Rating_Values check (rating between 1 and 5) not null,
	foreign key user_id references Users(id),
	foreign key photo_id references Photos(id),
	primary key (user_id, photo_id)
);

create table Tags (
	id 			serial,
	freq 		integer constraint Positive_Freq check (freq >= 0) not null,
	"name"		NameValue not null,
	primary key (id),
);

create table Users_Photos_has_Tags (
	when_taggged 		timestamp not null,
	user_id 			integer,
	photo_id			integer,
	tag_id				integer,
	foreign key user_id references Users(id),
	foreign key photo_id references Photos(id),
	foreign key tag_id references Tags(id),
	primary key (user_id, photo_id, tag_id)
);

create table Collections (
	id 				serial,
	title 			varchar(50) not null,
	has_key			integer,
	description		text,
	foreign key has_key references Photos(id),
	primary key (id)
);

create table Photos_in_Collections (
	-- photo 1 in group null is not allowed
	photo_id			integer not null,
	collection_id 		integer not null,
	"order" 			smallint not null
						constraint valid_order check ("order" > 0)
	foreign key photo_id references Photos(id),
	foreign key collection_id references Collections(id),
	primary key (photo_id, collection_id)
);

create table Users_Collections (
	user_id 		integer,
	owned_by		integer,
	foreign key user_id references Users(id),
	foreign key owned_by references Collections(id),
	primary key (user_id, owned_by)
);

create table Groups_Collections (
	group_id 		integer,
	owned_by		integer,
	foreign key group_id references Groups(id),
	foreign key owned_by references Collections(id),
	primary key (user_id, owned_by)
);

create table Comments (
	id 					serial,
	when_posted 		timestamp not null,
	authored_by			integer not null,
	contained_in 		integer not null,
	replyTo 			integer,
	foreign key authored_by references Users(id),
	foreign key replyTo references Comments(id),
	foreign key contained_in references Discussions(id) not null,
	primary key (id)
);

create table Discussions_has_Groups (
	group_id 			integer,
	discussion_id 		integer,
	foreign key (group_id) references Groups(id),
	foreign key (discussion_id) references Discussions(id),
	primary key (group_id, discussion_id)
);