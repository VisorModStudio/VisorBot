#include "BulkMessageDelete.h"

std::string BulkMessageDelete::getName() const
{
    return "BulkMessageDelete";
}
std::string BulkMessageDelete::getDescription() const
{
    return "Deletes a custom amount of messages all at once";

}

void BulkMessageDelete::execute(const dpp::slashcommand_t& event)
{
    //TODO
}

