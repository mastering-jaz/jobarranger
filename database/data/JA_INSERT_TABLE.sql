
-- Job Arranger initial data register SQL - 2013/05/24 -

-- Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.


START TRANSACTION;

DELETE FROM ja_define_value_jobcon_table;
DELETE FROM ja_index_table;
DELETE FROM ja_parameter_table;
DELETE FROM ja_host_lock_table;

INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('CURRENT_TIME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOBNET_BOOT_TIME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOBNET_ID');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOBNET_NAME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOB_EXIT_CD');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOB_ID');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOB_NAME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('LAST_STATUS');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('STD_ERR');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('STD_OUT');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('USER_NAME');

INSERT INTO ja_index_table (count_id, nextid) VALUES (1, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (2, 1500000000000000000);
INSERT INTO ja_index_table (count_id, nextid) VALUES (3, 1600000000000000000);
INSERT INTO ja_index_table (count_id, nextid) VALUES (20, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (30, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (100, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (101, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (102, 1);

INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_KEEP_SPAN', '60');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_LOAD_SPAN', '60');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBLOG_KEEP_SPAN', '1440');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_START_X', '117');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_START_Y', '39');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_JOB_X', '117');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_JOB_Y', '93');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_END_X', '117');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_END_Y', '146');

INSERT INTO ja_host_lock_table (lock_host_name) VALUES ('HOST_LOCK_RECORD');

COMMIT;
