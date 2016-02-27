<?php

/**
 *
 */
function hook_paynl_payment_current_country_alter(&$country, $payment) {
  // Always use The Netherlands as current country.
  return 'NL';
}

/**
 *
 */
function hook_paynl_payment_transaction_data_alter($transaction_data, Payment $payment) {
  // Add the internal payment id to the transaction data.
  // This is an example that won't do anything in the Pay.nl transaction.
  $transaction_data['pid'] = $payment->pid;
}
