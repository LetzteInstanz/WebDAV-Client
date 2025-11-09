#include "QtUtil.h"

QString to_qstring(QNetworkReply::NetworkError error) {
    assert(error != QNetworkReply::NoError);
    QString display_str;
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
            display_str = QObject::tr("Connection refused");
            break;

        case QNetworkReply::RemoteHostClosedError:
            display_str = QObject::tr("Connection closed");
            break;

        case QNetworkReply::HostNotFoundError:
            display_str = QObject::tr("Host not found");
            break;

        case QNetworkReply::TimeoutError:
            display_str = QObject::tr("Connection timed out");
            break;

        case QNetworkReply::SslHandshakeFailedError:
            display_str = QObject::tr("SSL/TLS handshake failed");
            break;

        case QNetworkReply::TemporaryNetworkFailureError:
            display_str = QObject::tr("Connection broken");
            break;

        case QNetworkReply::NetworkSessionFailedError:
            display_str = QObject::tr("Session failed");
            break;

        case QNetworkReply::BackgroundRequestNotAllowedError:
            display_str = QObject::tr("Background request not allowed");
            break;

        case QNetworkReply::TooManyRedirectsError:
            display_str = QObject::tr("Too many redirects");
            break;

        case QNetworkReply::InsecureRedirectError:
            display_str = QObject::tr("Insecure redirect");
            break;

        case QNetworkReply::UnknownNetworkError:
            display_str = QObject::tr("Unknown network error");
            break;

        case QNetworkReply::ProxyConnectionRefusedError:
            display_str = QObject::tr("Connection to proxy refused");
            break;

        case QNetworkReply::ProxyConnectionClosedError:
            display_str = QObject::tr("Connection to proxy closed");
            break;

        case QNetworkReply::ProxyNotFoundError:
            display_str = QObject::tr("Proxy not found");
            break;

        case QNetworkReply::ProxyTimeoutError:
            display_str = QObject::tr("Connection to proxy timed out");
            break;

        case QNetworkReply::ProxyAuthenticationRequiredError:
            display_str = QObject::tr("Proxy requires authentication");
            break;

        case QNetworkReply::UnknownProxyError:
            display_str = QObject::tr("Unknown proxy error");
            break;

        case QNetworkReply::ContentAccessDenied:
            display_str = QObject::tr("Access to content denied");
            break;

        case QNetworkReply::ContentOperationNotPermittedError:
            display_str = QObject::tr("Content operation not permitted");
            break;

        case QNetworkReply::ContentNotFoundError:
            display_str = QObject::tr("Content not found");
            break;

        case QNetworkReply::AuthenticationRequiredError:
            display_str = QObject::tr("Authentication required");
            break;

        case QNetworkReply::ContentReSendError:
            display_str = QObject::tr("Request resend failed");
            break;

        case QNetworkReply::ContentConflictError:
            display_str = QObject::tr("Conflict error");
            break;

        case QNetworkReply::ContentGoneError:
            display_str = QObject::tr("Content no longer available");
            break;

        case QNetworkReply::UnknownContentError:
            display_str = QObject::tr("Unknown content error");
            break;

        case QNetworkReply::ProtocolUnknownError:
            display_str = QObject::tr("Protocol unknown error");
            break;

        case QNetworkReply::ProtocolInvalidOperationError:
            display_str = QObject::tr("Invalid operation for protocol");
            break;

        case QNetworkReply::ProtocolFailure:
            display_str = QObject::tr("Protocol handle error");
            break;

        case QNetworkReply::InternalServerError:
            display_str = QObject::tr("Internal server error");
            break;

        case QNetworkReply::OperationNotImplementedError:
            display_str = QObject::tr("Server does not support operation");
            break;

        case QNetworkReply::ServiceUnavailableError:
            display_str = QObject::tr("Service unavailable");
            break;

        case QNetworkReply::UnknownServerError:
            display_str = QObject::tr("Unknown server error");
            break;

        default:
            display_str = QObject::tr("Unknown error");
            break;
    }
    return display_str;
}
