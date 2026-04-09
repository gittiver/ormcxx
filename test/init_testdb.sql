create database test_db;
create user testuser with encrypted password 'mypassword';
grant all privileges on database test_db to testuser;
