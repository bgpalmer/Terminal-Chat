#include "channel.hpp"


Channel::Channel( const char * name ) {
	std::memset( name_, '\0', sizeof( name_ ));
	std::strcpy( name_, name );
}

const char * Channel::name( void ) const {
	return name_;
}

Channel::~Channel( void ) { }

void Channel::join( boost::shared_ptr<Session> session ) { 
	connections_.insert( session );
}

void Channel::leave( boost::shared_ptr<Session> session ) {
	connections_.erase( session );
}

void Channel::deliver( Messages msg ) {

	ChannelUtility deliverer;

	/* Add message to chat log */
	log_.push_back( msg );

	/* Deliver message to all users in channel */
	for ( auto connection: connections_ )
		deliverer.send_msg( connection, msg );



	// /* Log will only hold Channel::log_max messages */
	// if ( log_.size() > Channel::log_max )
	// 	log_.pop_front();
}


