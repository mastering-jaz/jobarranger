
-- Job Arranger initial data register SQL - 2014/02/13 -

-- Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.


START TRANSACTION;

DELETE FROM ja_define_value_jobcon_table;
DELETE FROM ja_define_extjob_table;
DELETE FROM ja_index_table;
DELETE FROM ja_parameter_table;
DELETE FROM ja_host_lock_table;
DELETE FROM ja_define_run_log_message_table;

INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('CURRENT_TIME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOBNET_BOOT_TIME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOBNET_ID');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOBNET_NAME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOB_EXIT_CD');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOB_ID');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('JOB_NAME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('LAST_STATUS');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('MANAGEMENT_ID');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('STD_ERR');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('STD_OUT');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('USER_NAME');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('ZBX_DATA_TYPE');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('ZBX_LAST_STATUS');
INSERT INTO ja_define_value_jobcon_table (value_name) VALUES ('ZBX_LATEST_DATA');

INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdsleep', 'en_gb', 'Sleep until the specified time', 'I will wait for the process only during the specified number of seconds. Please specify the time in seconds waiting for the parameter.');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdtime', 'en_gb', 'Waiting until the specified time', 'I will wait for the process until the specified time. Please specify in the format HHMM the time waiting for the parameter.');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdweek', 'en_gb', 'Check the day of the week', 'We will determine whether to match the day the day of the week during the run icon is specified. Please specify (Sun Mon Tue Wed Thu Fri Sat) days of the week that you want to compare the parameter. The day of the week can be specified more than once.');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('zabbix_sender', 'en_gb', 'Issue the zabbix_sender', 'I do call Zabbix sender. Please specify the parameters to be passed to the command the parameter zabbix_sender. Example: -z zabbix_hostname -p zabbix_port_number -s host_name -k item_key -o "value"');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdsleep', 'en_us', 'Sleep until the specified time', 'I will wait for the process only during the specified number of seconds. Please specify the time in seconds waiting for the parameter.');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdtime', 'en_us', 'Waiting until the specified time', 'I will wait for the process until the specified time. Please specify in the format HHMM the time waiting for the parameter.');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdweek', 'en_us', 'Check the day of the week', 'We will determine whether to match the day the day of the week during the run icon is specified. Please specify (Sun Mon Tue Wed Thu Fri Sat) days of the week that you want to compare the parameter. The day of the week can be specified more than once.');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('zabbix_sender', 'en_us', 'Issue the zabbix_sender', 'I do call Zabbix sender. Please specify the parameters to be passed to the command the parameter zabbix_sender. Example: -z zabbix_hostname -p zabbix_port_number -s host_name -k item_key -o "value"');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdsleep', 'ja_jp', '時間待合せ（SLEEP）', '指定された秒数の間だけ処理を待合せします。パラメータに待合せ時間を秒単位(0～999999)で指定してください。');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdtime', 'ja_jp', '時刻待合せ（TIME）', '指定された時刻まで処理を待合せします。パラメータに待合せ時刻を HHMM(0000～9959) の形式で指定してください。');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('jacmdweek', 'ja_jp', '曜日判断', 'アイコン実行時の曜日が指定された曜日と一致するかを判断します。パラメータに比較したい曜日(Sun, Mon, Tue, Wed, Thu, Fri, Sat)を指定してください。曜日は複数指定が可能です。');
INSERT INTO ja_define_extjob_table (command_id, lang, command_name, memo) VALUES ('zabbix_sender', 'ja_jp', 'Zabbix通知（zabbix_sender）', 'Zabbix senderの呼出しを行います。パラメータに zabbix_sender コマンドに渡すパラメータを指定してください。 例：-z Zabbixホスト名 -p Zabbixポート番号 -s ホスト名 -k アイテムキー -o "通知内容（値）"');

INSERT INTO ja_index_table (count_id, nextid) VALUES (1, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (2, 1500000000000000000);
INSERT INTO ja_index_table (count_id, nextid) VALUES (3, 1600000000000000000);
INSERT INTO ja_index_table (count_id, nextid) VALUES (20, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (30, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (40, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (100, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (101, 1);
INSERT INTO ja_index_table (count_id, nextid) VALUES (102, 1);

INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBLOG_KEEP_SPAN', '43200');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_KEEP_SPAN', '60');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_LOAD_SPAN', '60');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_VIEW_SPAN', '60');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_START_X', '117');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_START_Y', '39');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_JOB_X', '117');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_JOB_Y', '93');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_END_X', '117');
INSERT INTO ja_parameter_table (parameter_name, value) VALUES ('JOBNET_DUMMY_END_Y', '146');

INSERT INTO ja_host_lock_table (lock_host_name) VALUES ('HOST_LOCK_RECORD');

INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000001', 'en_gb', 'Jobnet has started.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000002', 'en_gb', 'Jobnet has ended.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000003', 'en_gb', 'Job has started.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000004', 'en_gb', 'Job has ended.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000005', 'en_gb', 'Job has timed out.', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000006', 'en_gb', 'Skip the job.', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000007', 'en_gb', 'Job is rerun.', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000001', 'en_gb', 'Failed to schedule start-up of jobnet.', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000002', 'en_gb', 'Job failed.', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000003', 'en_gb', 'Jobnet failed.', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000001', 'en_us', 'Jobnet has started.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000002', 'en_us', 'Jobnet has ended.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000003', 'en_us', 'Job has started.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000004', 'en_us', 'Job has ended.', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000005', 'en_us', 'Job has timed out.', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000006', 'en_us', 'Skip the job.', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000007', 'en_us', 'Job is rerun.', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000001', 'en_us', 'Failed to schedule start-up of jobnet.', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000002', 'en_us', 'Job failed.', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000003', 'en_us', 'Jobnet failed.', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000001', 'ja_jp', 'ジョブネットが開始しました。', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000002', 'ja_jp', 'ジョブネットが終了しました。', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000003', 'ja_jp', 'ジョブが開始しました。', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000004', 'ja_jp', 'ジョブが終了しました。', 0);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000005', 'ja_jp', 'ジョブがタイムアウトしました。', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000006', 'ja_jp', 'ジョブをスキップします。', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC00000007', 'ja_jp', 'ジョブを再実行します。', 1);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000001', 'ja_jp', 'ジョブネットのスケジュール起動が行えませんでした。', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000002', 'ja_jp', 'ジョブが異常終了しました。', 2);
INSERT INTO ja_define_run_log_message_table (message_id, lang, message, log_type) VALUES ('JC90000003', 'ja_jp', 'ジョブネットが異常終了しました。', 2);

COMMIT;
