#include "BulkMessageDelete.h"

std::string BulkMessageDelete::getName() const
{
    return "BulkMessageDelete";
}
std::string BulkMessageDelete::getDescription() const
{
    return "Deletes messages in bulk that were posted in a certain time range";

}

void BulkMessageDelete::execute(const dpp::slashcommand_t& event)
{
    //TODO
}

