// -*- c++ -*-

#ifndef CERTIFICATEREQUESTBUILDER_P_H
#define CERTIFICATEREQUESTBUILDER_P_H

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include "certificaterequestbuilder.h"

struct CertificateRequestBuilderPrivate
{
    int errno;
    gnutls_x509_crq_t crq;
};

#endif // CERTIFICATEREQUESTBUILDER_P_H
