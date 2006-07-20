#include "bio_coords.h"

void free_bs(bio_struct *bs)
{
    if (bs->target_count>0)
    {
        free(bs->target_names);
        free(bs->targetsRSA);
        free(bs->targetsCRD);
    }
}
