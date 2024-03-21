//
// Created by Lo Yu Sum on 2024/03/19.
//

#include "ServerSectionGroup.hpp"
#include "GroupParticipant.hpp"

void
SanguisNet::GroupParticipant::TransferSection( std::shared_ptr<ServerSectionGroup> Group )
{
    auto SharedFromThis = shared_from_this( );

    if ( m_BelongsTo ) m_BelongsTo->Leave( SharedFromThis );
    m_BelongsTo = std::move( Group );
    if ( m_BelongsTo ) m_BelongsTo->Join( SharedFromThis );
}
