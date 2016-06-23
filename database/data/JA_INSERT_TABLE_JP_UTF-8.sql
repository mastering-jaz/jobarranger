
-- Job Arranger initial data register SQL (for Japanese) - 2012/12/05 -

-- Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.


START TRANSACTION;

DELETE FROM ja_define_extjob_table;

INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('jacmdsleep', '時間待合せ（SLEEP）', '指定された秒数の間だけ処理を待合せします。パラメータに待合せ時間を秒単位(0～999999)で指定してください。');
INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('jacmdtime', '時刻待合せ（TIME）', '指定された時刻まで処理を待合せします。パラメータに待合せ時刻を HHMM(0000～9959) の形式で指定してください。');
INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('jacmdweek', '曜日判断', 'アイコン実行時の曜日が指定された曜日と一致するかを判断します。パラメータに比較したい曜日(Sun, Mon, Tue, Wed, Thu, Fri, Sat)を指定してください。曜日は複数指定が可能です。');
INSERT INTO ja_define_extjob_table (command_id, command_name, memo) VALUES ('zabbix_sender', 'Zabbix通知（zabbix_sender）', 'Zabbix senderの呼出しを行います。パラメータに zabbix_sender コマンドに渡すパラメータを指定してください。 例：-z Zabbixホスト名 -p Zabbixポート番号 -s ホスト名 -k アイテムキー -o "通知内容（値）"');

COMMIT;
