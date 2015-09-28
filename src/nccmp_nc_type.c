#include <log.h>
#include <nccmp_error.h>
#include <nccmp_nc_type.h>
#include <string.h>

void nccmp_nc_type_name_to_str(nc_type type, char* str, int ncid, int debug)
{
    int status;
    if (debug) {
        LOG_DEBUG("ncid = %d, type = %d\n", ncid, type);
    }

    switch(type) {
        case NC_NAT:    strcpy(str, "NOT-A-TYPE");  break;
        case NC_BYTE:   strcpy(str, "BYTE");        break;
        case NC_CHAR:   strcpy(str, "CHAR");        break;
        case NC_DOUBLE: strcpy(str, "DOUBLE");      break;
        case NC_FLOAT:  strcpy(str, "FLOAT");       break;
        case NC_INT:    strcpy(str, "INT");         break;
        case NC_INT64:  strcpy(str, "INT64");       break;
        case NC_SHORT:  strcpy(str, "SHORT");       break;
        case NC_STRING: strcpy(str, "STRING");      break;
        case NC_UBYTE:  strcpy(str, "UBYTE");       break;
        case NC_UINT:   strcpy(str, "UINT");        break;
        case NC_UINT64: strcpy(str, "UINT64");      break;
        case NC_USHORT: strcpy(str, "USHORT");      break;
        default:
            status = nc_inq_type(ncid, type, str, 0 /*size*/);
            HANDLE_NC_ERROR(status);
            break;
    }
}
