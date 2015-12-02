<?php

/**
 * This file contains classes for the Pay.nl Payment module.
 */

/**
 * Pay.nl payment method controller.
 */
class PaynlPaymentMethodController extends PaymentMethodController {
  public $payment_method_configuration_form_elements_callback = 'paynl_payment_method_configuration';

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

    // Get line items and calculate amount.
    $line_items = array();
    $amount = 0;
    foreach ($payment->line_items as $line_item) {
      $line_items[] = array(
        'id' => $line_item->name,
        'name' => $line_item->name,
        'price' => (1 + $line_item->tax_rate) * $line_item->amount,
        'tax' => $line_item->tax_rate * $line_item->amount,
        'qty' => $line_item->quantity,
      );

      $amount += (1 + $line_item->tax_rate) * $line_item->amount
              * $line_item->quantity;
    }

    // Start transaction.
    $transaction = \Paynl\Transaction::start(array(
      'amount' => $amount,
      'returnUrl' => url(PAYNL_PAYMENT_RETURN_PATH . '/' . $payment->pid, array('absolute' => TRUE)),
      'exchangeUrl' => url(PAYNL_PAYMENT_LISTENER_PATH . '/' . $payment-> pid, array('absolute' => TRUE)),
      'description' => $payment->description,
      'products' => $line_items,
      'language' => strtoupper($language->language),
    ));

    // Redirect to PSP.
    $redirect_url = $transaction->getRedirectUrl();
    drupal_goto($redirect_url);
  }
}
