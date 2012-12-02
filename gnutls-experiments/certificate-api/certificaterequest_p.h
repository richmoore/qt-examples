// -*- c++ -*-

#ifndef CERTIFICATEREQUEST_P_H
#define CERTIFICATEREQUEST_P_H

#include "certificaterequest.h"

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

class CertificateRequestPrivate : public QSharedData
{
public:
    CertificateRequestPrivate();
    ~CertificateRequestPrivate();

    bool null;
    int errno;
    gnutls_x509_crq_t crq;
};

#endif // CERTIFICATEREQUEST_P_H
