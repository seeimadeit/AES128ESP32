#include "AES128ESP32.h"
// #include "psaerr2str.h"


static psa_status_t _psa_last_error = 0;

/** return the last error code*/
psa_status_t getLastAES128error() {
  return _psa_last_error;
}

/** convert the binary data into hex - output to the serial port, which could be a problem. expected to be used
for debugging only.
*/
void cryptodump(String title, unsigned char *buf, int lenbuff) {
  DEBUG2("lenbuff="+String(lenbuff));
  char hexbuffer[3];
  Serial.print(title);
  Serial.print(" ");
  for (int i = 0; i < lenbuff; i++) {
    sprintf(hexbuffer, "%02X", buf[i]);
    Serial.print(hexbuffer);
  }
  Serial.print(" ");
  for (int i = 0; i < lenbuff; i++) {
    if (buf[i] < 32)
      Serial.print(".");
    else if (buf[i] > 127)
      Serial.print(".");
    else {
      sprintf(hexbuffer, "%c", buf[i]);
      Serial.print(hexbuffer);
    }
  }
  Serial.println("");
}

/**
  convert an error code into a string. the error code is stored in _psa_last_error so it can be recovered with getLastAES128error().

information about psa can be found here : https://arm-software.github.io/psa-api/crypto/1.2/overview/intro.html



  known errors: (unknown errors are not converted to an human readable error string)
    PSA_SUCCESS
    PSA_ERROR_INSUFFICIENT_MEMORY
    PSA_ERROR_INSUFFICIENT_STORAGE
    PSA_ERROR_COMMUNICATION_FAILURE
    PSA_ERROR_HARDWARE_FAILURE
    PSA_ERROR_CORRUPTION_DETECTED
    PSA_ERROR_INSUFFICIENT_ENTROPY
    PSA_ERROR_STORAGE_FAILURE
    PSA_ERROR_DATA_INVALID
    PSA_ERROR_DATA_CORRUPT
    PSA_ERROR_BUFFER_TOO_SMALL
    PSA_ERROR_INVALID_ARGUMENT
    PSA_ERROR_BAD_STATE
    PSA_ERROR_INVALID_SIGNATURE
    PSA_ERROR_NOT_SUPPORTED
    PSA_ERROR_INVALID_HANDLE
    PSA_ERROR_INSUFFICIENT_DATA
    PSA_ERROR_NOT_PERMITTED
    AES128_ERROR1 bytes invalid size.
    AES128_ERROR2 newkeyid is not valid.
    AES128_ERROR3 masterkeyid is not valid.
    AES128_ERROR4 masterkey invalid size.
    AES128_ERROR5 masterkey has already been created.
    AES128_ERROR6 masterkey has not been set.
*/
String cryptoerrortoString(psa_status_t err) {
  DEBUG2(String(err));
  String rtn = "";
  _psa_last_error = err;
  switch (err) {
    // from psa_crypto_init
    case PSA_SUCCESS:
      rtn = "PSA_SUCCESS";
      break;
    case PSA_ERROR_INSUFFICIENT_MEMORY:
      rtn = "PSA_ERROR_INSUFFICIENT_MEMORY";
      break;
    case PSA_ERROR_INSUFFICIENT_STORAGE:
      rtn = "PSA_ERROR_INSUFFICIENT_STORAGE";
      break;
    case PSA_ERROR_COMMUNICATION_FAILURE:
      rtn = "PSA_ERROR_COMMUNICATION_FAILURE";
      break;
    case PSA_ERROR_HARDWARE_FAILURE:
      rtn = "PSA_ERROR_HARDWARE_FAILURE";
      break;
    case PSA_ERROR_CORRUPTION_DETECTED:
      rtn = "PSA_ERROR_CORRUPTION_DETECTED";
      break;
    case PSA_ERROR_INSUFFICIENT_ENTROPY:
      rtn = "PSA_ERROR_INSUFFICIENT_ENTROPY";
      break;
    case PSA_ERROR_STORAGE_FAILURE:
      rtn = "PSA_ERROR_STORAGE_FAILURE";
      break;
    case PSA_ERROR_DATA_INVALID:
      rtn = "PSA_ERROR_DATA_INVALID";
      break;
    case PSA_ERROR_DATA_CORRUPT:
      rtn = "PSA_ERROR_DATA_CORRUPT";
      break;
      // from psa_import_key
    // case PSA_ERROR_INSUFFICIENT_MEMORY: rtn = "PSA_ERROR_INSUFFICIENT_MEMORY"; break;
    // case PSA_ERROR_INSUFFICIENT_STORAGE: rtn = "PSA_ERROR_INSUFFICIENT_STORAGE"; break;
    // case PSA_ERROR_COMMUNICATION_FAILURE: rtn = "PSA_ERROR_COMMUNICATION_FAILURE"; break;
    // case PSA_ERROR_HARDWARE_FAILURE: rtn = "PSA_ERROR_HARDWARE_FAILURE"; break;
    // case PSA_ERROR_CORRUPTION_DETECTED: rtn = "PSA_ERROR_CORRUPTION_DETECTED"; break;
    // case PSA_ERROR_INSUFFICIENT_ENTROPY: rtn = "PSA_ERROR_INSUFFICIENT_ENTROPY"; break;
    // case PSA_ERROR_STORAGE_FAILURE: rtn = "PSA_ERROR_STORAGE_FAILURE"; break;
    // case PSA_ERROR_DATA_INVALID: rtn = "PSA_ERROR_DATA_INVALID"; break;
    // case PSA_ERROR_DATA_CORRUPT: rtn = "PSA_ERROR_DATA_CORRUPT"; break;

    // from psa_aead_update
    case PSA_ERROR_BUFFER_TOO_SMALL: rtn = "PSA_ERROR_BUFFER_TOO_SMALL"; break;
    case PSA_ERROR_INVALID_ARGUMENT: rtn = "PSA_ERROR_INVALID_ARGUMENT"; break;
    case PSA_ERROR_BAD_STATE: rtn = "PSA_ERROR_BAD_STATE"; break;

    // from psa_aead_finish
    case PSA_ERROR_INVALID_SIGNATURE:
      rtn = "PSA_ERROR_INVALID_SIGNATURE";
      break;
      // case PSA_ERROR_BUFFER_TOO_SMALL: rtn="PSA_ERROR_BUFFER_TOO_SMALL"; break;

    case PSA_ERROR_NOT_SUPPORTED:
      rtn = "PSA_ERROR_NOT_SUPPORTED";
      break;

      // from psa_export_key
      //  case PSA_SUCCESS: rtn="" ; break;
    case PSA_ERROR_INVALID_HANDLE:
      rtn = "PSA_ERROR_INVALID_HANDLE";
      break;
      //  case PSA_ERROR_INVALID_ARGUMENT: rtn="" ; break;
      //  *         The key is neither a public key nor a key pair.
      //  case PSA_ERROR_NOT_SUPPORTED: rtn="" ; break;
      //  case PSA_ERROR_BUFFER_TOO_SMALL: rtn="" ; break;
      /*         The size of the \p data buffer is too small. You can determine a
 *         sufficient buffer size by calling
 *         #PSA_EXPORT_KEY_OUTPUT_SIZE(#PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(\c type), \c bits)
 *         where \c type is the key type
 *         and \c bits is the key size in bits.
 */
      //  case PSA_ERROR_COMMUNICATION_FAILURE: rtn="" ; break;
      //  case PSA_ERROR_HARDWARE_FAILURE: rtn="" ; break;
      //  case PSA_ERROR_CORRUPTION_DETECTED: rtn="" ; break;
      //  case PSA_ERROR_STORAGE_FAILURE: rtn="" ; break;
      //  case PSA_ERROR_INSUFFICIENT_MEMORY: rtn="" ; break;
      //  case PSA_ERROR_BAD_STATE: rtn="" ; break;

    case PSA_ERROR_INSUFFICIENT_DATA: rtn = "PSA_ERROR_INSUFFICIENT_DATA"; break;
    case PSA_ERROR_NOT_PERMITTED: rtn = "PSA_ERROR_NOT_PERMITTED"; break;

      // AES128ESP32 Library errors

    case AES128_ERROR1:
      rtn = "AES128_ERROR1 bytes invalid size.";
      break;
    case AES128_ERROR2:
      rtn = "AES128_ERROR2 newkeyid is not valid.";
      break;
      case AES128_ERROR3:
      rtn = "AES128_ERROR3 masterkeyid is not valid.";
      break;
      case AES128_ERROR4:
      rtn = "AES128_ERROR4 masterkey invalid size.";
      break;
      case AES128_ERROR5:
      rtn = "AES128_ERROR5 masterkey has already been created.";
      break;
      case AES128_ERROR6:
      rtn = "AES128_ERROR6 masterkey has not been set.";
      break;

//// end of known errors
    default:
      rtn = "Unknown error " + String(err);
  }
  return rtn;
}
