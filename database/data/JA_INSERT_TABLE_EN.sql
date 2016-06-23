
-- Job Arranger initial data register SQL (for English) - 2012/12/05 -

-- Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.


START TRANSACTION;

DELETE FROM ja_define_extjob_table;

INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('jacmdsleep', 'Sleep until the specified time', 'I will wait for the process only during the specified number of seconds. Please specify the time in seconds waiting for the parameter.');
INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('jacmdtime', 'Waiting until the specified time', 'I will wait for the process until the specified time. Please specify in the format HHMM the time waiting for the parameter.');
INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('jacmdweek', 'Check the day of the week', 'We will determine whether to match the day the day of the week during the run icon is specified. Please specify (Sun Mon Tue Wed Thu Fri Sat) days of the week that you want to compare the parameter. The day of the week can be specified more than once.');
INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('zabbix_sender', 'Issue the zabbix_sender', 'I do call Zabbix sender. Please specify the parameters to be passed to the command the parameter zabbix_sender. Example: -z zabbix_hostname -p zabbix_port_number -s host_name -k item_key -o "value"');

COMMIT;
