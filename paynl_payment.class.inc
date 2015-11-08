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
    // Get configuration data.
    $controller_data = $payment->method->controller_data;

    // Configure API.
    $paynl_config = new \Paynl\Config();
    $api_key = $controller_data['paynl_id'];
    $service_id = $controller_data['paynl_service_id'];
    $paynl_config->setApiToken($api_key);
    $paynl_config->setServiceId($service_id);

    // Calculate amount.
    $amount = 0;
    foreach ($payment->line_items as $line_item) {
      $amount += (1 + $line_item->tax_rate) * $line_item->amount
              * $line_item->quantity;
    }

    // Start transaction.
    $paynl = new \Paynl\Api\Transaction\Start();

    // Set transaction data.
    $paynl->setAmount(100 * $amount);
    $paynl->setFinishUrl(url(PAYNL_PAYMENT_RETURN_PATH
              . '/' . $payment->pid, array('absolute' => TRUE)));
    $paynl->setExchangeUrl(url(PAYNL_PAYMENT_LISTENER_PATH
              . '/' . $payment-> pid, array('absolute' => TRUE)));
    $paynl->setDescription($payment->description);

    // Execute transaction request.
    $paynl_payment = $paynl->doRequest();

    // Redirect to PSP.
    $redirect_url = $paynl_payment['transaction']['paymentURL'];
    drupal_goto($redirect_url);
  }
}
