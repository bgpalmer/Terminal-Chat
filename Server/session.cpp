#include "session.hpp"
#include "channel.hpp"

Session::Session( Guest::pointer guest, tcp::socket socket, const short port, Channel::pointer channel )
    : socket_( std::move( socket )), room_( channel ), port_( port ), guest_( guest ), address_( guest->get_address().address(), port_ ) 
{
	std::cerr << "Session: In Constructor" << std::endl;
	this->read_msg.clear();
} 


Session::~Session( void ) { }

void Session::start() {
	do_connect_();
}

void Session::deliver( Messages & msg ) {
	std::cerr << "Session::deliver(): " << msg.get_body() << std::endl;
	//bool work = write_msg.empty();
	write_msg.push( msg );
	{
		std::cerr << "writing" << std::endl;
		do_write_header_();
	}
}

void Session::do_connect_( void ) {

	socket_.async_connect(
		address_,
		boost::bind(
			&Session::on_connect_,
			shared_from_this(),
			_1
		)
	);
}

void Session::on_connect_( error_code ec ) {
	std::cerr << "Connected to " << address_ << std::endl;
	if ( ! ec ) {
		room_->join( shared_from_this() );

		time_t current_time;
		Messages test("CHANNEL", "YOU HAVE JOINED", time(&current_time), MSG);

		write_msg.push(test);
	
		do_read_header_();
	} else {
		std::cerr << "Session::on_connect_(): " << ec.message() << std::endl;
	}
}

void Session::do_read_header_() {
	memset(read_buffer_, '\0', sizeof(char)*512);
	
	socket_.async_receive( boost::asio::buffer( read_buffer_, MAX_HEADER_LENGTH ), 
	boost::bind( &Session::on_read_header_, shared_from_this(), _1, _2));
}

void Session::on_read_header_( const boost::system::error_code error, size_t bytes ) {
	if ( ! error ) {
        std::string temp(read_buffer_);

		if (temp.length() == 0)
		{
			std::cout << temp.length() << std::endl;
			//do_read_header();
			return;
		}
        std::cerr << "Session::on_read_header(): " << temp << std::endl;
        this->read_msg.get_header() = std::move(temp);
		this->read_msg.parse_header();
		this->do_read_body_();
	}
	else
	{
		//room.leave(shared_from_this());
	}
}

// Will have multiple handlers depending on the command that was sent

void Session::do_read_body_(void)
{
	memset(read_buffer_, '\0', sizeof(char)*512);

	this->socket_.async_receive( boost::asio::buffer(read_buffer_, MAX_MSG_LENGTH), 
	boost::bind(&Session::on_read_body_, shared_from_this(), _1, _2));
}
// MSG
void Session::on_read_body_( const boost::system::error_code error, size_t bytes ) {
	if ( ! error ) {

        std::string temp(read_buffer_);

		std::cerr << "Session::on_read_body(): " << temp << std::endl;
        this->read_msg.get_body() = std::move(temp);
		this->write_msg.push( this->read_msg );

		do_write_header_();
		//room_->deliver( read_msg );
		do_read_header_();
	}
}

/* Instead of string, should be message class */
void Session::do_write_header_( void ) {
    std::string to_send = write_msg.front().get_header();
	std::cout << "HEADER TO SEND " << to_send << std::endl;
	
	boost::asio::async_write(socket_, boost::asio::buffer( to_send, to_send.length()), boost::bind( &Session::on_write_header_, shared_from_this(), _1, _2 ));
}

void Session::on_write_header_( const boost::system::error_code error, size_t bytes ) {
	if ( ! error ) {
		std::string to_send = write_msg.front().get_header();
		std::cout << "HEADER ON SEND " << to_send << std::endl;
		do_write_body_();
	}
}

void Session::do_write_body_()
{
	std::string to_send = write_msg.front().get_body();
	std::cout << "BODY TO SEND " << to_send << std::endl;

	boost::asio::async_write(socket_, boost::asio::buffer( to_send, to_send.length()), boost::bind( &Session::on_write_body_, shared_from_this(), _1, _2 ));
	
}

void Session::on_write_body_( const boost::system::error_code error, size_t bytes ) {
	std::string to_send = write_msg.front().get_body();
		std::cout << "BODY ON SEND " << to_send << std::endl;
	write_msg.pop();

	if ( ! error ) 
	{
		if(!write_msg.empty())
		{
			std::cout << "STILL MORE MESSAGES" << std::endl;
			do_write_header_();
		}	
	}
}


