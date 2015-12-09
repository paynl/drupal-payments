<?php

/**
 * This file contains classes for the Pay.nl Payment module.
 */

/**
 * Pay.nl payment method controller.
 */
class PaynlPaymentMethodController extends PaymentMethodController {
  public $payment_method_configuration_form_elements_callback = 'paynl_payment_method_configuration';
  public $payment_configuration_form_elements_callback = 'paynl_payment_payment_configuration';

  /**
   * Class constructor.
   */
  public function __construct() {
    $this->title = t('Pay.nl');
  }

  /**
   * Implements PaymentMethodController::execute().
   */
  public function execute(Payment $payment) {
    // Get the current language.
    global $language;

    // Get configuration data.
    $controller_data = $payment->method->controller_data;

    // Configure API.
    $api_key = $controller_data['paynl_id'];
    $service_id = $controller_data['paynl_service_id'];
    \Paynl\Config::setApiToken($api_key);
    \Paynl\Config::setServiceId($service_id);

    // Get line items to pass them to Pay.nl.
    $line_items = array();
    foreach ($payment->getLineItems() as $line_item) {
      $line_items[] = array(
        'id' => $line_item->name,
        'name' => $line_item->name,
        'price' => $line_item->unitAmount(TRUE),
        'tax' => $line_item->unitAmount(TRUE) - $line_item->unitAmount(FALSE),
        'qty' => $line_item->quantity,
      );
    }

    // Start transaction.
    $transaction = \Paynl\Transaction::start(array(
      'amount' => $payment->totalAmount(TRUE),
      'returnUrl' => url(PAYNL_PAYMENT_RETURN_PATH . '/' . $payment->pid, array('absolute' => TRUE)),
      'exchangeUrl' => url(PAYNL_PAYMENT_LISTENER_PATH . '/' . $payment-> pid, array('absolute' => TRUE)),
      'paymentMethod' => $payment->context_data['payment_method'],
      'description' => $payment->description,
      'products' => $line_items,
      'language' => strtoupper($language->language),
      'ipaddress' => ip_address(),
      'testmode' => $controller_data['paynl_test_mode'],
    ));

    // Redirect to PSP.
    $redirect_url = $transaction->getRedirectUrl();
    drupal_goto($redirect_url);
  }
}
