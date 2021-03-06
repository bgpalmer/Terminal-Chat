#include "messages.hpp"
#include <ctime>
#include <iostream>

Messages::Messages()
{
	this->sender_id = "";
	this->msg_body = "";
	this->timestamp = time(NULL);
	this->command = 0;
	this->body_length = 0;
	this->header = "";
}

Messages::Messages(std::string sender, std::string msg_body, time_t t, Commands command)
{
	this->sender_id = sender;
	this->msg_body = msg_body;
	this->timestamp = t;
	this->command = command;
	this->body_length = msg_body.length();
	
	if (this->command < 10)
	{
		this->header.append("0");
		this->header.append(std::to_string(command));
	}
	else
	{
		this->header.append(std::to_string(command));
	}

	if (this->body_length < 10)
	{
		this->header.append("00");
		this->header.append(std::to_string(this->body_length));
	}
	else if (this->body_length < 100)
	{
		this->header.append("0");
		this->header.append(std::to_string(this->body_length));
	}
	else
	{
		this->header.append(std::to_string(this->body_length));
	}

	this->header.append(std::to_string(this->timestamp));
	this->header.append(sender_id);
	
	for (int i = 0; i < (MAX_ALIAS_LENGTH - this->sender_id.length()); i++)
	{
		this->header.append("0");
	}
}

Messages::~Messages()
{
}

std::string Messages::get_sender()
{
	return this->sender_id;
}

std::string& Messages::get_body()
{
	return this->msg_body;
}

time_t Messages::get_time()
{
	return this->timestamp;
}

int Messages::get_command()
{
	return this->command;
}

int& Messages::get_length()
{
	return this->body_length;
}

std::string& Messages::get_header()
{
	return this->header;
}

void Messages::clear()
{
	this->sender_id = "";
	this->msg_body = "";
	this->header = "";
	this->timestamp = 0;
	this->command = 0;
	this->body_length = 0;
}

void Messages::parse_header()
{
	this->command = std::stoi(this->header.substr(0, 2));

	this->body_length = std::stoi(this->header.substr(2, 3));

	this->timestamp = std::stoi(this->header.substr(5, 15));

	this->sender_id = this->header.substr(15, 30);

	for (int i = 0; i < this->sender_id.length(); i++)
	{
		if (this->sender_id[i] == '0')
		{
			this->sender_id.erase(i, std::string::npos);
			return;
		}
	}
}