//
// Created by hackman on 4/25/25.
//

#ifndef BUILDER_H
#define BUILDER_H



namespace obelisk::database::query {


class builder {
protected:
    enum E_STATEMENT_TYPE
    {
        EST_QUERY,
        EST_UPDATE,
        EST_INSERT,
        EST_DELETE
    };


};

} // obelisk::database::query
#endif //BUILDER_H
